#pragma once
#include "define.h"

class Tracker
{
public:
	Tracker(void){HSVImg=NULL;};

	~Tracker(void){ReleaseHSV();};

	void getTrackingObject(IplImage* frame){
		char* win_name = "First frame";
		params p;
		int i, x1, y1, x2, y2, w, h;

		/* use mouse callback to allow user to define object regions */
		p.win_name = win_name;
		p.orig_img = (IplImage*)cvClone( frame );
		p.cur_img = NULL;
		p.n = 0;
		cvNamedWindow( win_name, 1 );
		cvShowImage( win_name, frame );

		cvSetMouseCallback( win_name, &mouse, &p );
		cvWaitKey( 0 );
		cvDestroyWindow( win_name );
		cvReleaseImage( &(p.orig_img) );
		if( p.cur_img )
			cvReleaseImage( &(p.cur_img) );

		/* extract regions defined by user; */
		if( p.n == 0 )
			return ;

		for( i = 0; i < p.n; i++ )
		{
			region.x = x1 = MIN( p.loc1[i].x, p.loc2[i].x );
			x2 = MAX( p.loc1[i].x, p.loc2[i].x );
			region.y = y1 = MIN( p.loc1[i].y, p.loc2[i].y );
			y2 = MAX( p.loc1[i].y, p.loc2[i].y );
			w = x2 - x1;
			h = y2 - y1;

			/* ensure odd width and height */
			region.width= ( w % 2 )? w : w+1;
			region.height = ( h % 2 )? h : h+1;
		}
	};

	void getLimit(IplImage* frame){
		char* win_name = "First frame";
		params p;
		int i, x1, y1, x2, y2, w, h;

		/* use mouse callback to allow user to define object regions */
		/*cvRectangle( frame, cvPoint( MIN(region.x,frame->width), MIN(region.y,frame->height) ), 
			cvPoint( MIN(region.x+region.width,frame->width), MIN(region.y+region.height,frame->height) ),
			CV_RGB(255,0,0), 5, 8, 0 );*/
		p.win_name = win_name;
		p.orig_img = (IplImage*)cvClone( frame );
		p.cur_img = NULL;
		p.n = 0;
		cvNamedWindow( win_name, 1 );
		cvShowImage( win_name, frame );

		cvSetMouseCallback( win_name, &mouse, &p );
		cvWaitKey( 0 );
		cvDestroyWindow( win_name );
		cvReleaseImage( &(p.orig_img) );
		if( p.cur_img )
			cvReleaseImage( &(p.cur_img) );

		/* extract regions defined by user; */
		if( p.n == 0 )
			return ;

		for( i = 0; i < p.n; i++ )
		{
			limit.x = x1 = MIN( p.loc1[i].x, p.loc2[i].x );
			x2 = MAX( p.loc1[i].x, p.loc2[i].x );
			limit.y = y1 = MIN( p.loc1[i].y, p.loc2[i].y );
			y2 = MAX( p.loc1[i].y, p.loc2[i].y );
			w = x2 - x1;
			h = y2 - y1;

			/* ensure odd width and height */
			limit.width= ( w % 2 )? w : w+1;
			limit.height = ( h % 2 )? h : h+1;
		}
	};
	void ReadAndChangeToHSV(IplImage* bgr){
		IplImage* bgr32f = cvCreateImage( cvGetSize(bgr), IPL_DEPTH_32F, 3 );
		this->HSVImg = cvCreateImage( cvGetSize(bgr), IPL_DEPTH_32F, 3 );
		cvConvertScale( bgr, bgr32f, 1.0 / 255.0, 0 );
		cvCvtColor( bgr32f, this->HSVImg, CV_BGR2HSV );
		cvReleaseImage( &bgr32f );
	};

	void ReleaseHSV(){
		if(HSVImg!=NULL)
			cvReleaseImage(&HSVImg);
	};
	static void getRect(IplImage* frame, CvRect& limit){
		char* win_name = "First frame";
		params p;
		int i, x1, y1, x2, y2, w, h;

		p.win_name = win_name;
		p.orig_img = (IplImage*)cvClone( frame );
		p.cur_img = NULL;
		p.n = 0;
		cvNamedWindow( win_name, 1 );
		cvShowImage( win_name, frame );

		cvSetMouseCallback( win_name, &mouse, &p );
		cvWaitKey( 0 );
		cvDestroyWindow( win_name );
		cvReleaseImage( &(p.orig_img) );
		if( p.cur_img )
			cvReleaseImage( &(p.cur_img) );

		/* extract regions defined by user; */
		if( p.n == 0 )
			return ;

		for( i = 0; i < p.n; i++ )
		{
			limit.x = x1 = MIN( p.loc1[i].x, p.loc2[i].x );
			x2 = MAX( p.loc1[i].x, p.loc2[i].x );
			limit.y = y1 = MIN( p.loc1[i].y, p.loc2[i].y );
			y2 = MAX( p.loc1[i].y, p.loc2[i].y );
			w = x2 - x1;
			h = y2 - y1;

			/* ensure odd width and height */
			limit.width= ( w % 2 )? w : w+1;
			limit.height = ( h % 2 )? h : h+1;
		}
	};
	virtual void getResult(CvRect& rect)=0;

	CvRect& getRegion(){return region;};
	CvRect& getLimitRegion(){return limit;};
protected:
	//HSV Image
	IplImage* HSVImg;

	CvRect region;

	CvRect limit;

	typedef struct params {
		CvPoint loc1[MAX_OBJECTS];
		CvPoint loc2[MAX_OBJECTS];
		IplImage* objects[MAX_OBJECTS];
		char* win_name;
		IplImage* orig_img;
		IplImage* cur_img;
		int n;
	} params;

	static void mouse( int event, int x, int y, int flags, void* param ){
		params* p = (params*)param;
		CvPoint* loc;
		int n;
		IplImage* tmp;
		static int pressed = FALSE;

		/* on left button press, remember first corner of rectangle around object */
		if( event == CV_EVENT_LBUTTONDOWN )
		{
			n = p->n;
			if( n == MAX_OBJECTS )
				return;
			loc = p->loc1;
			loc[n].x = x;
			loc[n].y = y;
			pressed = TRUE;
		}

		/* on left button up, finalize the rectangle and draw it in black */
		else if( event == CV_EVENT_LBUTTONUP )
		{
			n = p->n;
			if( n == MAX_OBJECTS )
				return;
			loc = p->loc2;
			loc[n].x = x;
			loc[n].y = y;
			cvReleaseImage( &(p->cur_img) );
			p->cur_img = NULL;
			cvRectangle( p->orig_img, p->loc1[n], loc[n], CV_RGB(0,0,0), 1, 8, 0 );
			cvShowImage( p->win_name, p->orig_img );
			pressed = FALSE;
			p->n++;
		}

		/* on mouse move with left button down, draw rectangle as defined in white */
		else if( event == CV_EVENT_MOUSEMOVE  &&  flags & CV_EVENT_FLAG_LBUTTON )
		{
			n = p->n;
			if( n == MAX_OBJECTS )
				return;
			tmp = (IplImage*)cvClone( p->orig_img );
			loc = p->loc1;
			cvRectangle( tmp, loc[n], cvPoint(x, y), CV_RGB(255,255,255), 1, 8, 0 );
			cvShowImage( p->win_name, tmp );
			if( p->cur_img )
				cvReleaseImage( &(p->cur_img) );
			p->cur_img = tmp;
		}
	};
	
};
