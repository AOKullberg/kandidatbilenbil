/* FIL sensormodul.cpp
 * Huvudfil för sensormodulen. Initierar kommunikation med dator
 * och startar sen bildbehandling, kompression etc.
 */
#include <wiringPi.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <opencv2/core/core.hpp>
#include <raspicam/raspicam_cv.h>
#include "Bildbehandling/bildbehandling.hpp"
#include "Kommunikation/kommunikation_klient.hpp"
#include "Compression/compression.hpp"
#include "Sensorer/sensorer.hpp"

#define PI 3.141592

using namespace std;
using namespace cv;
using namespace sf;

/* Globala variabler
 */
bool end_program = false;
 
//För bildbehandling och kompression
Mat image;
Mat processed_image(480,640,CV_8UC3);
condition_variable img_cv;
condition_variable data_cv;
mutex mut_image;
mutex mut_data;
bool data_ready = false;
bool img_ready = false;

//För kommunikation med styr/kom
int sign_found = 0;
condition_variable dist_cv;
mutex mut_dist;
bool dist_ready = false;

//För kommunikation med dator
extern TcpSocket socket;
compression wifi_data;

mutex mut_camera;
condition_variable camera_cv;
bool camera_ready = false;

void restart()
{
	system("/home/pi/Desktop/Bilen_Original/restart");
}

int main()
{
	/* Initierar kameran & sätter parametrar.
	 * VGA-upplösning, 640x480, används för att minska
	 * belastningen på processorn.
	 */
	raspicam::RaspiCam_Cv Camera;
	Camera.set(CV_CAP_PROP_FORMAT,CV_8UC3);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH,320);
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT,240);
	if(!Camera.open())
	{
		cerr << "Error opening camera";
		thread t(restart);
		t.detach();
		return 1;
	}
	
	/* Initierar wifi-anslutningen till datorn. Förutsätter
	 * att de är anslutna till ett ad-hoc-nätverk.
	 *//*
	if(init_connection() != Socket::Done)
	{
		cerr << "Can't connect to host\n" << "Please restart the application\n";
	}
	else
	{
		cout << "Connection established!\n";
	}*/
	
	if(sensor_setup() == -1)
	{
		cerr << "Error setting up sensors\n" << "Please restart the application\n";
	}
	//thread kommunikation(send_image);
	thread bildbehandling(img_proc_main);
	//thread img_compression(img_comp_main);
	thread speed(measure_speed);
	speed.detach();
	
	/* Huvudloop
	 * Tar en bild från kameran och skapar sedan en bildbehandlingstråd
	 */
	while(1)
	{
		unique_lock<mutex> lk(mut_camera);
		while(camera_ready)
		{
			camera_cv.wait(lk);
		}
		Camera.grab();
		Camera.retrieve(image);
		camera_ready=true;
		camera_cv.notify_one();
		read_sensors();
		send_sensor_data();
		
		
	/* Väntar på att användaren trycker på esc i ett visst antal ms.
	 * Avbryter då loopen.
	 */
		if(waitKey(30)==27)
		{
			break;
		}
	}
	
	/* Släpper kamerafeeden och stänger programmet.
	 */
	end_program = true;
	bildbehandling.join();
	//kommunikation.join();
	//img_compression.join();
	//speed.join();
	socket.disconnect();
	Camera.release();
	return 0;
}
