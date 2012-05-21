/********************************* Structures ********************************/
#include <Windows.h>
#include <cvaux.h>
#include <cv.h>
/********************************* Functions ********************************/
void getRegions( IplImage* frame, CvRect& rect );
void mouse( int event, int x, int y, int flags, void* param );
IplImage* bgr2hsv( IplImage* bgr );
#define X 0
#define Y 1
#define S 2
int mainForHybridV2()
{
	bool showAll = false;
	string fileName="../car.AVI";
	CvCapture* video=NULL;
	IplImage* frame=NULL;
	IplImage* hsvImg=NULL;
	vector<IplImage*> show;

	//read video
	video = cvCaptureFromFile(fileName.data());
	if( ! video )
		fatal_error("couldn't open video file: "+fileName);

	//--------------get regions
	//Note: frame 不需要调用release，cvQueryFrame返回的图像不可以被用户释放或者修改

	int skip=0;
	while(skip<30){
		frame = cvQueryFrame( video );
		skip++;
	}

	show.push_back((IplImage *)cvClone( frame ));

	CvRect rect;
	getRegions(frame,rect);
	
	hsvImg=bgr2hsv(frame);
	CHSVHistogram orgHisto(frame,rect);
	cvReleaseImage(&hsvImg);
	
	int DP=3;
	int MP=3;
	int SamplesNum=20;
	CvConDensation* particleFilter = cvCreateConDensation(DP,MP,SamplesNum);

	CvMat* lowerBound;    // 下界
	CvMat* upperBound;    // 上界
	lowerBound = cvCreateMat(3, 1, CV_32F);
	upperBound = cvCreateMat(3, 1, CV_32F);

	cvmSet( lowerBound, X, 0, 0.0 ); 
	cvmSet( lowerBound, Y, 0, 0.0 ); 
	cvmSet( lowerBound, S, 0, 0.0 );

	cvmSet( upperBound, X, 0, frame->width );
	cvmSet( upperBound, Y, 0, frame->height);
	cvmSet( upperBound, S, 0, 2.0);

	cvConDensInitSampleSet(particleFilter,lowerBound,upperBound);

	for(int i=0; i < SamplesNum; i++){
		particleFilter->flSamples[i][X]+=rect.x;
		particleFilter->flSamples[i][Y]+=rect.y;
		particleFilter->flSamples[i][S]+=1;
	}

	particleFilter->DynamMatr[0]=1.0;particleFilter->DynamMatr[1]=0.0;
	particleFilter->DynamMatr[2]=0.0;particleFilter->DynamMatr[3]=1.0;

	cvNamedWindow( "Video", 1 );
	DWORD start=0,end=0;
	int i=0;

	int x0,y0,w,h;

	while (frame = cvQueryFrame( video ))
	{
		start=::GetTickCount();
		show.push_back((IplImage *)cvClone( frame ));
		hsvImg=bgr2hsv(frame);

		for(int i=0; i < SamplesNum; i++){
			particleFilter->flConfidence[i] = orgHisto.likehood(hsvImg,particleFilter->flSamples[i][Y], particleFilter->flSamples[i][X], 
				rect.width * particleFilter->flSamples[i][S],
				rect.height * particleFilter->flSamples[i][S]);
		}

		cvConDensUpdateByTime(particleFilter);

		x0=y0=w=h=0;
		for(int i=0; i < SamplesNum; i++){
			y0+=particleFilter->flSamples[i][Y];
			x0+=particleFilter->flSamples[i][X];
			w+=rect.width * particleFilter->flSamples[i][S];
			h+=rect.height * particleFilter->flSamples[i][S];
		}
		x0/=SamplesNum;
		y0/=SamplesNum;
		w/=SamplesNum;
		h/=SamplesNum;

		cvReleaseImage(&hsvImg);
		end=::GetTickCount();
		cout<<"time elapsed "<<end-start<<endl;

		cvRectangle( show[i], cvPoint( x0, y0 ), cvPoint( x0+w, y0+h ), CV_RGB(255,0,0), 1, 8, 0 );

		cvShowImage( "Video", show[i] );

		cvWaitKey( 5 );

		i++;
	}

	//--------------end
	cvReleaseCapture( &video );

	for(int j = 0; j < show.size(); j++ )
	{
		cvReleaseImage( &(show[j]) );
	}

	cvWaitKey(0);
	return 0;
}
/*
Allows the user to interactively select object regions.

@param frame the frame of video in which objects are to be selected
@param regions a pointer to an array to be filled with rectangles
defining object regions

@return Returns the number of objects selected by the user
*/
void getRegions( IplImage* frame, CvRect& rect )
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
		rect.x = x1 = MIN( p.loc1[i].x, p.loc2[i].x );
		x2 = MAX( p.loc1[i].x, p.loc2[i].x );
		rect.y = y1 = MIN( p.loc1[i].y, p.loc2[i].y );
		y2 = MAX( p.loc1[i].y, p.loc2[i].y );
		w = x2 - x1;
		h = y2 - y1;

		/* ensure odd width and height */
		rect.width = w = ( w % 2 )? w : w+1;
		rect.height = h = ( h % 2 )? h : h+1;
	}
}

IplImage* bgr2hsv( IplImage* bgr )
{
	IplImage* bgr32f, * hsv;

	bgr32f = cvCreateImage( cvGetSize(bgr), IPL_DEPTH_32F, 3 );
	hsv = cvCreateImage( cvGetSize(bgr), IPL_DEPTH_32F, 3 );
	cvConvertScale( bgr, bgr32f, 1.0 / 255.0, 0 );
	cvCvtColor( bgr32f, hsv, CV_BGR2HSV );
	cvReleaseImage( &bgr32f );

	return hsv;
}