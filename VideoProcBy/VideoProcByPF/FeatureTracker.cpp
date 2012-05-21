#include "FeatureTracker.h"

FeatureTracker::FeatureTracker(void)
{
	tmp =NULL;
	eig_image=NULL;
	tmp_image=NULL;
}

FeatureTracker::~FeatureTracker(void)
{
	if(eig_image!=NULL)
		cvReleaseImage(&eig_image);

	if(tmp_image!=NULL)
		cvReleaseImage(&tmp_image);

	if(tmp!=NULL)
		cvReleaseImage(&tmp);
}
void FeatureTracker::initialize(IplImage* raw)
{
	midPosi=0;
	isFirst=0;
	bool success=false;
	do{
		cout<<"Select ROI"<<endl;
		getTrackingObject(raw);
		cvSetImageROI(raw,region);
		cout<<"Finish ROI selection, please select the limit"<<endl;

		cout<<"Select Limit"<<endl;
		getLimit(raw);
		cout<<"Finish limit selection, please select the standard line"<<endl;

		tmp=cvCreateImage(cvGetSize(raw),IPL_DEPTH_8U,1);

		midPosi=getLine(raw);

		cvLine( raw, cvPoint(0,midPosi),cvPoint(region.width,midPosi),CV_RGB(0,255,0), 1, CV_AA, 0 );
		cvRectangle( raw, cvPoint( limit.x, limit.y) , 
			cvPoint( limit.x+limit.width, limit.y+limit.height ),
			CV_RGB(0,255,0), 5, 8, 0 );

		cvShowImage("Video",raw);

		cout<<"Finish standard line selection, press r to re-select, other to continue"<<endl;

		int flag=cvWaitKey(0);
		if(flag==(int)'r' || flag==(int)'R')
		{
			cout<<"reselect region"<<endl;
			success=false;
			cvReleaseImage(&tmp);
			cvReleaseImage(&eig_image);
			cvReleaseImage(&tmp_image);
			cvResetImageROI( raw );
		}
		else{
			cvResetImageROI( raw );
			eig_image=cvCreateImage(cvGetSize(raw),IPL_DEPTH_32F,1);
			tmp_image=cvCreateImage(cvGetSize(raw),IPL_DEPTH_32F,1);
			cout<<eig_image->width<<' '<<eig_image->height;	
			success=true;
		}
	}while(success=false);
};
void FeatureTracker::getPoint(IplImage* frame,CvPoint& point )
{
	cvSetImageROI(frame,region);

	cvConvertImage(frame,tmp);
	//cvCvtColor(frame,tmp,CV_RGB2GRAY);

	cvErode(tmp,tmp,0,2);

	cvDilate(tmp,tmp,0,2);

	cvCanny(tmp, tmp, 50, 250, 3 );

	cvDilate(tmp,tmp,0,2);

	cvErode(tmp,tmp,0,2);

	posi=GetCornerPointInRect(tmp);

	if(posi.x!=-1)
	{
		if(isFirst!=0)
		{
			posi.x=posi.x*(1-ALPHA_RESULT)+prePosi.x*ALPHA_RESULT;
			posi.y=posi.y*(1-ALPHA_RESULT)+prePosi.y*ALPHA_RESULT;
			if(abs(posi.x-prePosi.x)>frame->width/20 || abs(posi.y-prePosi.y)>frame->height/20)
			{
				posi.x=prePosi.x;
				posi.y=prePosi.y;
			}
		}
		prePosi=posi;
		isFirst=1;
		point=posi;
	}
	else
	{
		cout<<"error, keep unmoved"<<endl;
		point=prePosi;
	}
};
CvPoint FeatureTracker::GetCornerPointInRect(IplImage* raw)
{
	int corner_count=500;
	IplImage*ImageROI=cvCreateImage(cvGetSize(raw),IPL_DEPTH_8U,1);  
	cvZero(ImageROI);  
	cvSetImageROI(ImageROI, limit);  
	cvSet(ImageROI,cvScalar(255));   
	cvResetImageROI(ImageROI);  

	cvGoodFeaturesToTrack(raw,eig_image,tmp_image,cornersA,&corner_count,0.01,10,ImageROI);

	CvPoint a;
	a.x=a.y=0;
	double max=DBL_MIN;
	int index=-1;
	if(corner_count!=0){
		for (int i=0;i<corner_count;i++)
		{
			a.x+=cornersA[i].x;
			a.y+=cornersA[i].y;
		}
		a.x/=corner_count;
		a.y/=corner_count;
	}
	else
	{
		a.x=a.y=-1;
	}
	cvReleaseImage(&ImageROI);
	return a;
}
void FeatureTracker::getPointLine(IplImage* frame,CvPoint& point,CvMemStorage* storage)
{
	cvSetImageROI(frame,region);

	cvConvertImage(frame,tmp);

	cvErode(tmp,tmp,0,2);

	cvDilate(tmp,tmp,0,6);

	cvThreshold(tmp,tmp,120,255, CV_THRESH_BINARY);//110
	cvShowImage("Video",tmp);
	cvWaitKey(0);

	cvCanny(tmp, tmp, 50, 250, 3 );
	cvDilate(tmp,tmp,0,3);
	cvShowImage("Video",tmp);
	cvWaitKey(0);

	posi.x=posi.y=0;
	CvSeq* lines = cvHoughLines2( tmp, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 100, 0, 0 );
	
	if(lines->total!=0)
	{
		for(int  i = 0; i < lines->total; i++ )
		{
			float* line = (float*)cvGetSeqElem(lines,i);
			float rho = line[0];
			float theta = line[1];
			CvPoint pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			//posi.x += (cvRound(x0 + 1000*(-b)) + cvRound(x0 - 1000*(-b)))/2;
			posi.y += (cvRound(y0 + 1000*(a)) + cvRound(y0 - 1000*(a)))/2;
			/*cvLine( frame, cvPoint(cvRound(x0 + 1000*(-b)),cvRound(y0 + 1000*(a))),
				cvPoint(cvRound(x0 - 1000*(-b)),cvRound(y0 - 1000*(a))),
				CV_RGB(255,0,0), 1, CV_AA, 0 );*/
		}
		//posi.x/=lines->total;
		posi.y/=lines->total;	
		posi.x=limit.x+limit.width/2;

		if(isFirst!=0)
		{
			posi.x=posi.x*(1-ALPHA_RESULT)+prePosi.x*ALPHA_RESULT;
			posi.y=posi.y*(1-ALPHA_RESULT)+prePosi.y*ALPHA_RESULT;
			if(abs(posi.x-prePosi.x)>frame->width/20 || abs(posi.y-prePosi.y)>frame->height/20)
			{
				posi.x=prePosi.x;
				posi.y=prePosi.y;
			}
		}
		prePosi=posi;
		isFirst=1;
		point=posi;
	}
	else
	{
		cout<<"error, keep unmoved"<<endl;
		point=prePosi;
	}
};
void FeatureTracker::getPointPic(IplImage* frame,CvPoint& point,int Thresh )
{
	cvSetImageROI(frame,region);

	cvConvertImage(frame,tmp);
	//cvCvtColor(frame,tmp,CV_RGB2GRAY);

	cvErode(tmp,tmp,0,2);

	cvDilate(tmp,tmp,0,2);

	cvThreshold(tmp,tmp,Thresh,255, CV_THRESH_BINARY);//110
	cvCanny(tmp, tmp, 50, 250, 3 );

	cvDilate(tmp,tmp,0,2);

	cvErode(tmp,tmp,0,2);

	posi=GetCornerPointInRect(tmp);

	if(posi.x!=-1)
	{
		if(isFirst!=0)
		{
			posi.x=posi.x*(1-ALPHA_RESULT)+prePosi.x*ALPHA_RESULT;
			posi.y=posi.y*(1-ALPHA_RESULT)+prePosi.y*ALPHA_RESULT;
			if(abs(posi.x-prePosi.x)>frame->width/20 || abs(posi.y-prePosi.y)>frame->height/20)
			{
				posi.x=prePosi.x;
				posi.y=prePosi.y;
			}
		}
		prePosi=posi;
		isFirst=1;
		point=posi;
	}
	else
	{
		cout<<"error, keep unmoved"<<endl;
		point=prePosi;
	}
};
int FeatureTracker::getLine( IplImage* frame )
{
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
	cvSetMouseCallback( win_name, &mouseLine, &p );
	cvWaitKey( 0 );
	cvDestroyWindow( win_name );
	cvReleaseImage( &(p.orig_img) );
	if( p.cur_img )
		cvReleaseImage( &(p.cur_img) );

	/* extract regions defined by user; */
	if( p.n == 0 )
		return -1;

	for( i = 0; i < p.n; i++ )
	{
		x1 = MIN( p.loc1[i].x, p.loc2[i].x );
		x2 = MAX( p.loc1[i].x, p.loc2[i].x );
		y1 = MIN( p.loc1[i].y, p.loc2[i].y );
		y2 = MAX( p.loc1[i].y, p.loc2[i].y );
	}
	return (y1+y2)/2;
}
void FeatureTracker::mouseLine( int event, int x, int y, int flags, void* param )
{
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
		cvLine( p->orig_img, p->loc1[n], loc[n], CV_RGB(0,0,0), 1, 8, 0 );
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
		cvLine( tmp, loc[n], cvPoint(x, y), CV_RGB(255,255,255), 1, 8, 0 );
		cvShowImage( p->win_name, tmp );
		if( p->cur_img )
			cvReleaseImage( &(p->cur_img) );
		p->cur_img = tmp;
	}
}
void FeatureTracker::relocate(IplImage* raw)
{
	isFirst=0;
	cvSetImageROI(raw,region);
	getLimit(raw);
};