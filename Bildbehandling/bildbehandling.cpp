#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "bildbehandling.hpp"

#define PI 3.141592

using namespace std;
using namespace cv;

/*Externa variabler
 */
extern bool end_program;
//För kommunikation med huvudloop & dator
extern Mat image;
extern Mat processed_image;
extern bool img_ready;
extern condition_variable img_cv;
extern mutex mut_image;
extern mutex mut_camera;
extern condition_variable camera_cv;
extern bool camera_ready;

//För kommunikation med styr/kom
extern int dist_to_side;
extern int sign_found;

/*Interna globala variabler.
 */
int horizon_y = 150;
int dist_to_right = 0;
int dist_to_left = 0;
Rect sign_roi = Rect(320,0,320,240);
Rect line_roi = Rect(0,150,640,330);
bool proc_sync = true;
condition_variable sync_cv;
mutex sync_mut;

int img_proc_main()
{
	/* Skapar en "worker thread" som behandlar en bild. Börjar
	 * sedan själv att behandla en annan bild.
	 * Syncas med hjälp av mutex/condition variables
	 */
	thread worker(process_image,1);
	process_image(0);
	worker.join();
	return 0;
}


/* Huvudloop för bildbehandlingen
 */
int process_image(int id)
{
	while(1)
	{
	//namedWindow("Window",WINDOW_AUTOSIZE);
	/* Till för att synka mellan de två olika workertrådarna.
	 */
		unique_lock<mutex> lk1(sync_mut);
		switch(id)
		{
			case 0:
			while(proc_sync)
			{
				sync_cv.wait(lk1);
			}
			break;
			case 1:
			while(!proc_sync)
			{
				sync_cv.wait(lk1);
			}
			break;
		}
		
		Mat final_image, src, dst;
		
		/* Synkar hämtandet av kamerabilden mellan bildbehandlingen
		 * och huvudloopen
		 */
		unique_lock<mutex> lk(mut_camera);
		while(!camera_ready)
		{
			camera_cv.wait(lk);
		}
		src = image;
		camera_ready = false;
		camera_cv.notify_one();
		
		/* Låter detect_signs köras på egen parallell tråd och fortsätter
		 * sedan den nuvarande tråden.
		 */
		future<Mat> fut = async(detect_signs,src);
		
		dst = detect_edges(src);

		switch(id)
		{
			case 0:
			proc_sync = true;
			break;
			case 1:
			proc_sync = false;
			break;
		}
		sync_cv.notify_one();
		
		final_image = detect_lines(src, dst);
		fut.wait();
		Mat found_signs = fut.get();
		
		/* Lägger ihop de detekterade linjerna med den detekterade
		 * skylten.
		 */
		addWeighted(final_image(sign_roi), 1, found_signs, 1, 0.0, 
					final_image(sign_roi));
					
		unique_lock<mutex> lk2(mut_image);
		processed_image = final_image;
		img_ready = true;
		img_cv.notify_one();
		if(end_program)
		{
			return id;
		}
	}
	//imshow("Window",final_image);
}

/* FUNKTION detect_edges(Mat)
 * Använder en canny-detektor för att hitta kanter i bilden.
 * Returnerar en binär mat.
 */
Mat detect_edges(Mat src)
{
	Mat dst;
	Mat src_gray;
	
	int lowThreshold = 40;
	int ratio = 4;
	int kernel_size = 3;
	
	cvtColor(src,src_gray,CV_BGR2GRAY);
	
	/* Skapar en "destinationsmatris" i storleken av ROI
	 * dit den kant-detekterade bilden sparas.
	 */
	dst.create(src_gray.size(),src_gray.type());
	
	/* Detekterar kanter m.h.a. Canny-detektorn i enbart det
	 * fördefinierade ROI
	 */
	Canny(src_gray, dst, lowThreshold, lowThreshold*ratio, kernel_size);
	
	return dst;
}

/* FUNKTION detect_lines()
 * Detekterar linjer i bilden som sedan används till diverse
 * beräkningar.
 */
Mat detect_lines(Mat src, Mat dst)
{
	Mat cdst;

	/* Skapar en region of interest(ROI) för sidlinjer
	 */
	Rect roi = line_roi;
	
	/* Skapar en ny destinationsmatris i storleken av ROI dit
	 * linjer kommer målas ut av hough-transformen.
	 */
	cdst.create(src(roi).size(),src.type());
	
	/* Vektor som innehåller start och slutpunkter för alla 
	 * detekterade linjer.
	 * En vektor av vektorer av ints (Vec4i = vector<int,4>)
	 */
	vector<Vec4i> lines{};
	
	/* Detekterar linjer utifrån Canny-bilden.
	 */
	HoughLinesP(dst(roi),lines, 1, CV_PI/180, 60, 50, 10);
	
	/* Ritar ut de detekterade linjerna
	 */
	if(!lines.empty())
	{
		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];
			line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0), 3, 8);
		}
	}
	
	/* Plockar ut enbart den gröna kanalen av cdst.
	 * Skickar denna till distance_line som beräknar avståndet
	 * från höger respektive vänster kant.
	 */
	Mat green_dst = split_to_green(cdst);
	distance_to_line(green_dst(Range(230,330),Range(0,320)),0);
	distance_to_line(green_dst(Range(230,330),Range(320,640)),1);
	
	/* Adderar ihop cdst med ROI:t av src. Båda viktas 1.
	 */
	addWeighted(src(roi),1,cdst,1,0.0,src(roi));
	return src;
}

/* FUNKTION detect_signs(Mat)
 * Detekterar skyltar i högra hörnet av bilden.
 * Returnerar en mat innehållande en"cirkel" kring den funna skylten.
 */
Mat detect_signs(Mat dst)
{
	Rect roi = sign_roi;
	Mat cdst;
	cdst.create(dst(roi).size(),dst.type());
	cdst.setTo(Scalar(0,0,0));
	
	sign_found = 0;
	
	Mat dst_gray = dst(roi);
	
	cvtColor(dst(roi),dst_gray,CV_BGR2GRAY);
	
	GaussianBlur(dst_gray,dst_gray,Size(3,3),0,0);
	
	vector<Vec3f> circles;
	
	HoughCircles(dst_gray, circles, CV_HOUGH_GRADIENT, 1, dst_gray.rows, 30, 100, 0,0);
	
	for(size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		
		circle( cdst, center, radius, Scalar(0,0,255), 3, 8, 0 );
	}
	
	if(circles.size() > 0)
	{
		sign_found = 1;
	}
	
	return cdst;
}

/* FUNKTION: split_to_green(Mat)
 * Splittar upp en bild i sina respektive kanaler.
 * Returnerar den gröna kanalen.
 */
Mat split_to_green(Mat img)
{
	vector<Mat> channels{};
	split(img, channels);
	return channels[1];
}

/* FUNKTION: distance_line(Mat, int)
 * In: Mat - Område som skall kollas,
 *     int - 0 = vänster, 1 = höger
 * Beräknar avståndet till sidlinjer genom att
 * iterera genom ett litet område av bilden.
 */
void distance_to_line(Mat img, int left_right)
{
	int i = 0;
	MatIterator_<uchar> it, end;
	for(it = img.begin<uchar>(), end = img.end<uchar>(); it!= end; ++it)
	{
		if(*it != 0)
		{
			if(left_right == 0)
			{
				dist_to_left = 320 - i;
			}
			else
			{
				dist_to_right = i;
			}
			break;
		}
		else
		{
			++i;
		}
	}
}
