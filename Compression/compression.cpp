#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "compression.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace sf;
using namespace cv;

/* Externa globala variabler.
 */
extern Mat processed_image;
extern mutex mut_data;
extern mutex mut_image;
extern condition_variable img_cv;
extern condition_variable data_cv;
extern bool data_ready;
extern bool img_ready;
extern compression wifi_data;
extern bool end_program;

bool comp_sync = false;
mutex comp_mut;
condition_variable comp_cv;

int img_comp_main()
{
	/* Skapar en till tråd som kan komprimera bilder.
	 * Då finns alltid en tråd ledig när bildbehandlingen är redo.
	 */
	thread worker(compress_image,1);
	compress_image(0);
	worker.join();
	return 0;
}

int compress_image(int id)
{
	while(1)
	{
		/* Synkar med bildbehandlingen.
		 */
		unique_lock<mutex> lk(mut_image);
		while(!img_ready)
		{
			img_cv.wait(lk);
		}
		Mat img = processed_image;
		img_ready = false;
		img_cv.notify_one();
		
		/* Konverterar "Mat" från BGR till RGBA för att lättare kunna
		 * skicka bilden (SFML använder RGBA).
		 */
		cvtColor(img, img, COLOR_BGR2RGBA);
		size_t cols = img.cols;
		size_t rows = img.rows;
		uLong originalSize = cols*rows*4;
		byte* dataOriginal = img.ptr();
		
		uLong sizeComp = (originalSize * 1.1) + 12;
		
		/* Allokerar minnesplats för den komprimerade bilden.
		 */
		byte* dataComp = (byte*)malloc(sizeComp);
		
		/* Komprimerar bilden. 
		 * In: dataComp - dit datan sparas
		 * sizeComp - referens till storleken av den komprimerade datan
		 * dataOriginal - datan som skall komprimeras
		 * originalSize - storleken innan komprimering
		 */
		int z_result = compress(dataComp, &sizeComp, dataOriginal, originalSize);
		
		switch(z_result)
		{
			case Z_OK:
			cout << "Successfully compressed\n";
			break;
			case Z_MEM_ERROR:
			cerr << "Mem_error\n";
			break;
			case Z_BUF_ERROR:
			cerr << "Buffer error\n";
			break;
		}
		
		/* Skapar en struct där data som skall sändas sparas undan.
		 */
		compression comp;
		comp.data = dataComp;
		comp.comp_size = sizeComp;
		comp.sizeX = cols;
		comp.sizeY = rows;
		
		/* Synkar med kommunikationen.
		 */
		unique_lock<mutex> lk2(mut_data);
		while(data_ready)
		{
			data_cv.wait(lk2);
		}
		wifi_data = comp;
		data_ready = true;
		data_cv.notify_one();
		
		if(end_program)
		{
			return id;
		}
	}
}

