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
extern int sign_found;
extern mutex mut_dist;
extern condition_variable dist_cv;
extern bool dist_ready;


/*Interna globala variabler.
 */
int horizon_y = 75;
int dist_to_right = 0;
int dist_to_left = 0;
int dist_to_stop = 0;
Vec4i stop_line;
Rect sign_roi = Rect(160,0,160,120);
Rect line_roi = Rect(0,80,320,50);
Rect stop_roi_l = Rect(70,90,40,30);
Rect stop_roi_r = Rect(210,90,40,30);
int proc_sync = 0;
condition_variable sync_cv;
mutex sync_mut;

int img_proc_main()
{
	
	/* Skapar en "worker thread" som behandlar en bild. Börjar
	 * sedan själv att behandla en annan bild.
	 * Syncas med hjälp av mutex/condition variables
	 */
	thread worker(process_image,1);
	thread worker2(process_image,2);
	thread worker3(process_image,3);
	process_image(0);
	worker.join();
	worker2.join();
	worker3.join();
	return 0;
}


/* Huvudloop för bildbehandlingen
 */
int process_image(int id)
{
	while(1)
	{
	
	/* Till för att synka mellan de två olika workertrådarna.
	 */
	 
		unique_lock<mutex> lk1(sync_mut);
		switch(id)
		{
			case 0:
			while(proc_sync!=0)
			{
				sync_cv.wait(lk1);
			}
			break;
			case 1:
			while(proc_sync!=1)
			{
				sync_cv.wait(lk1);
			}
			break;
			case 2:
			while(proc_sync!=2)
			{
				sync_cv.wait(lk1);
			}
			break;
			case 3:
			while(proc_sync!=3)
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
		//future<Mat> fut = async(detect_signs,src);
		
		dst = detect_edges(src);
		
		switch(id)
		{
			case 0:
			proc_sync = 1;
			break;
			case 1:
			proc_sync = 2;
			break;
			case 2:
			proc_sync = 3;
			break;
			case 3:
			proc_sync = 0;
			break;
		}
		sync_cv.notify_all();
		
		final_image = detect_lines(src, dst);
		//fut.wait();
		//Mat found_signs = fut.get();
		
		/* Lägger ihop de detekterade linjerna med den detekterade
		 * skylten.
		 */
		//addWeighted(final_image(sign_roi), 1, found_signs, 1, 0.0, 
		//			final_image(sign_roi));
					
		unique_lock<mutex> lk2(mut_image);
		processed_image = final_image;
		img_ready = true;
		img_cv.notify_one();
		if(end_program)
		{
			return id;
		}
		int xs = 0;
		int xe = 320;
		int ys = line_roi.y;
		int ye = line_roi.y + line_roi.height;
		
		int xstop_l = stop_roi_l.x;
		int xstopend_l = stop_roi_l.x + stop_roi_l.width;
		int ystop_l = stop_roi_l.y;
		int ystopend_l = stop_roi_l.y + stop_roi_l.height;
		
		int xstop_r = stop_roi_r.x;
		int xstopend_r = stop_roi_r.x + stop_roi_r.width;
		int ystop_r = stop_roi_r.y;
		int ystopend_r = stop_roi_r.y + stop_roi_r.height;
			
		line( final_image, Point(xs,ys), Point(xe,ys), Scalar(255,0,0),3,8);
		line( final_image, Point(xs,ye), Point(xe,ye), Scalar(255,0,0),3,8);
		line( final_image, Point(xstop_l,ystop_l), Point(xstop_l,ystopend_l), Scalar(255,255,0),3,8);
		line( final_image, Point(xstopend_l,ystop_l), Point(xstopend_l,ystopend_l), Scalar(255,255,0),3,8);
		line(final_image, Point(xstop_l,ystopend_l),Point(xstopend_l,ystopend_l), Scalar(255,255,0),3,8);
		line( final_image, Point(xstop_r,ystop_r), Point(xstop_r,ystopend_r), Scalar(255,255,0),3,8);
		line( final_image, Point(xstopend_r,ystop_r), Point(xstopend_r,ystopend_r), Scalar(255,255,0),3,8);
		line(final_image, Point(xstop_r,ystopend_r),Point(xstopend_r,ystopend_r), Scalar(255,255,0),3,8);
		
		namedWindow("Window",WINDOW_AUTOSIZE);
		imshow("Window",final_image);
	}
	
}

/* FUNKTION detect_edges(Mat)
 * Använder en canny-detektor för att hitta kanter i bilden.
 * Returnerar en binär mat.
 */
Mat detect_edges(Mat src)
{
	Mat dst;
	Mat blur;
	Mat src_gray;
	Mat zero_Roi;
	
	int lowThreshold = 20;
	int ratio = 3;
	int kernel_size = 3;
	

	//namedWindow("Window 1",WINDOW_AUTOSIZE);
	//namedWindow("Window 2",WINDOW_AUTOSIZE);
	cvtColor(src,src_gray,CV_BGR2GRAY);
	
	/* Skapar en "destinationsmatris" i storleken av ROI
	 * dit den kant-detekterade bilden sparas.
	 */
	blur.create(src_gray.size(),src_gray.type());
	dst.create(src_gray.size(),src_gray.type());
	
	GaussianBlur( src_gray, blur, Size( 0, 0), 3, 3);
	//imshow("Window 1",blur);
	/* Detekterar kanter m.h.a. Canny-detektorn i enbart det
	 * fördefinierade ROI
	 */
	Canny(blur, dst, lowThreshold, lowThreshold*ratio, kernel_size);
	zero_Roi=dst(Rect(125,0,70,240));
	zero_Roi.setTo(0);
	//imshow("Window 2",dst);
	return dst;
}

/* FUNKTION detect_lines()
 * Detekterar linjer i bilden som sedan används till diverse
 * beräkningar.
 */
Mat detect_lines(Mat src, Mat dst)
{
	Mat cdst,stopdst_l,stopdst_r;
	int dist_to_stop_l;
	int dist_to_stop_r;
	int line_det_max_y = line_roi.height;
	int line_det_min_y = 0;
	
	/* Skapar en ny destinationsmatris i storleken av ROI dit
	 * linjer kommer målas ut av hough-transformen.
	 */
	stopdst_l.create(src(stop_roi_l).size(),src.type());
	stopdst_l.setTo(Scalar(0,0,0));
	stopdst_r.create(src(stop_roi_r).size(),src.type());
	stopdst_r.setTo(Scalar(0,0,0));	
	cdst.create(src(line_roi).size(),src.type());
	cdst.setTo(Scalar(0,0,0));
	
	/* Vektor som innehåller start och slutpunkter för alla 
	 * detekterade linjer.
	 * En vektor av vektorer av ints (Vec4i = vector<int,4>)
	 */
	vector<Vec4i> lines{};
	
	/* Detekterar linjer utifrån Canny-bilden.
	 */
	HoughLinesP(dst(line_roi),lines, 1, CV_PI/180, 15, 3, 3);
	/* Ritar ut de detekterade linjerna
	 */
	if(!lines.empty())
	{
		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];
			
			int ystart = l[1];
			int yend = l[3];
			
			if(ystart - 10 <= yend && yend <= ystart + 10)
			{
				//line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255),3,8);
			}
			else
			{
			//lines består av 4 ints. xstart, ystart, xend, yend
				line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0), 3, 8);
			}
		}
	}
	HoughLinesP(dst(stop_roi_l),lines,1,CV_PI/180,24,4,4);
	
	if(!lines.empty())
	{
		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];

			//lines består av 4 ints. xstart, ystart, xend, yend
				line( stopdst_l, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, 8);
		}
	}
	
	HoughLinesP(dst(stop_roi_r),lines,1,CV_PI/180,20,4,4);
	
	if(!lines.empty())
	{
		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];

			//lines består av 4 ints. xstart, ystart, xend, yend
				line( stopdst_r, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, 8);
		}
	}
	
	/* Plockar ut en kanal av cdst.
	 * Skickar denna till distance_line som beräknar avståndet
	 * från höger respektive vänster kant.
	 *//*
	unique_lock<mutex> lk1(mut_dist);
	if(dist_ready)
	{
		dist_cv.wait(lk1);
	}*/
	Mat green_dst = split_to_channel(cdst,'g');
	Mat red_dst_l = split_to_channel(stopdst_l,'r');
	Mat red_dst_r = split_to_channel(stopdst_r,'r');

	get_distance_to_line(green_dst(Range(line_det_min_y,line_det_max_y),Range(0,140)),0);
	get_distance_to_line(green_dst(Range(line_det_min_y,line_det_max_y),Range(180,320)),1);
	if(countNonZero(red_dst_l)>0)
	{
		dist_to_stop_l=get_distance_to_stop(red_dst_l(Range(0,red_dst_l.rows),Range(0,stop_roi_l.width)));
	}
	else
	{
		dist_to_stop_l = 0;
	}
	if(countNonZero(red_dst_r)>0)
	{
		dist_to_stop_r=get_distance_to_stop(red_dst_r(Range(0,red_dst_r.rows),Range(0,stop_roi_r.width)));
	}
	else
	{
		dist_to_stop_r = 0;
	}
	//cout<<"Höger: " << dist_to_stop_r << " Vänster: " << dist_to_stop_l << endl;
	if((dist_to_stop_r!=0)&&(dist_to_stop_l!=0))
	{
		dist_to_stop=(dist_to_stop_l+dist_to_stop_r)/2;
	}
	else
	{
		dist_to_stop=0;
	}
	/*
	if(stop_line[0] == -20 )
	{
		get_distance_to_stop();
		stop_line[0] = (-20);
	}*/
	
	dist_ready = true;
	dist_cv.notify_one();
	/* Adderar ihop cdst med ROI:t av src. Båda viktas 1.
	 */
	addWeighted(src(line_roi),1,cdst,1,0.0,src(line_roi));
	addWeighted(src(stop_roi_l),1,stopdst_l,1,0.0,src(stop_roi_l));
	addWeighted(src(stop_roi_r),1,stopdst_r,1,0.0,src(stop_roi_r));
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
	/*
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
	}*/
	
	return cdst;
}

/* FUNKTION: split_to_channel(Mat, char)
 * Splittar upp en bild i sina respektive kanaler.
 * Returnerar önskad kanal. Om "fel" värde skickas in returneras
 * ursprungsbilden
 */
Mat split_to_channel(Mat img, char channel)
{
	vector<Mat> channels{};
	split(img, channels);
	switch(channel)
	{
		case 'r':
		return channels[2];
		case 'b':
		return channels[0];
		case 'g':
		return channels[1];
	}
	return img;
}

/* FUNKTION: distance_line(Mat, int)
 * In: Mat - Område som skall kollas,
 *     int - 0 = vänster, 1 = höger
 * Beräknar avståndet till sidlinjer genom att
 * iterera genom ett litet område av bilden.
 */
void get_distance_to_line(Mat img, int left_right)
{	
	if(left_right)
	{
		dist_to_right = 70;
	}
	else
	{
		dist_to_left = 70;
	}
	int i = 0;
	MatIterator_<uchar> it, end;

		for(it = img.begin<uchar>(), end = img.end<uchar>(); it!= end; ++it)
		{
			if(*it != 0)
			{
				if(left_right)
				{
					dist_to_right = 20+ i;
					dist_to_right = (dist_to_right/3.73);
					//dist_to_right = (dist_to_right/4.225);
					break;
				}
				else
				{
					dist_to_left = 160 - i;
					dist_to_left = (dist_to_left/3.55);
					//dist_to_left = (dist_to_left/4.225);
					break;
				}
			}
			else
			{
				++i;
				if(i>=140)
				{
					i = 0;
				}
			}
		}
}

int get_distance_to_stop(Mat img)
{
	Mat tmp = img;
	int dist_to_stop;
	int cols = img.cols;
	tmp.create(img.cols,img.rows, img.type());
	int y = 0;
	transpose(img,tmp);
	int i = 0;
	MatIterator_<uchar> it, end;
	
	for( it = tmp.begin<uchar>(), end = tmp.end<uchar>(); it!= end; ++it)
	{
		if(*it != 0)
		{
			y = i;
			//cout << y << endl;
			dist_to_stop = 80-y;
			dist_to_stop = (dist_to_stop/1.487);
			//dist_to_stop = (dist_to_stop/2.65);
			break;
		}
		++i;
		if(i>cols)
			{
				i = 0;
			}
	}
	return dist_to_stop;
}
