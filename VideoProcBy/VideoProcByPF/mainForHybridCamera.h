#include "HybridTracker.h"
/********************************* Structures ********************************/
#include <Windows.h>
#include <fstream>
/********************************* Functions ********************************/
void showImage(IplImage* frame);
void getLimitRegion(IplImage* frame, CvRect& limit);

int mainForHybridCamera()
{
	bool showAll = false;
	string fileName="../real1.avi";
	CvCapture* video=NULL;
	IplImage* frame=NULL;
	IplImage *raw=NULL;

	char readVideo='y';
	cout<<"read video[y] or read camera[any key]";
	cin>>readVideo;
	
	if(readVideo=='y')
	{
		if( !(video = cvCaptureFromFile(fileName.data())))//cvCreateCameraCapture(-1)))
		{
			cout<<"Can not read video.\n";
			getchar();
			return -2;
		}
	}
	else
	{
		if( !(video = cvCreateCameraCapture(-1)))
		{
			cout<<"Can not open camera.\n";
			getchar();
			return -2;
		}
	}
	//--------------get regions
	//Note: frame 不需要调用release，cvQueryFrame返回的图像不可以被用户释放或者修改

	int skip=0;
	while(skip<30){
		frame = cvQueryFrame( video );
		skip++;
	}
	CvSize dstSize;
	dstSize = cvGetSize(frame);
	dstSize.height*=SCALE;
	dstSize.width*=SCALE;

	raw=cvCreateImage(dstSize,frame->depth,frame->nChannels);
	cvResize(frame,raw);

	cvNamedWindow( "Video", 1 );
	DWORD start=0,end=0;
	int i=0;

	CvRect result;
	CvPoint center;
	CvPoint preCenter;

	FILE* file = fopen( "../recordHybrid.txt", "w" );
	if( ! file )
		return 0;

	fstream data;
	data.open("../data.txt",ios::trunc|ios::in|ios::out,0x20);

	HybridTracker tracker;
	tracker.initialize(raw,PARTICLES_FOR_HYBRID);

	int key;

	CvRect LimitRegion;
	Tracker::getRect(raw, LimitRegion);

	int moveCount=0;

	while (frame = cvQueryFrame( video ))
	{
		start=::GetTickCount();
		
		cvResize(frame,raw);
		tracker.ReadAndChangeToHSV(raw);

		tracker.transition();

		//start=::GetTickCount();
		//tracker.updateWeights();
		//end=::GetTickCount();
		//cout<<"V1 outside time elapsed "<<end-start<<endl;

		//start=::GetTickCount();
		//tracker.updateWeightsV2();
		//end=::GetTickCount();
		//cout<<"V2 outside time elapsed "<<end-start<<endl;

		//start=::GetTickCount();
		tracker.updateWeightsV3();
		//end=::GetTickCount();
		//cout<<"V3 outside time elapsed "<<end-start<<endl;

		tracker.normalizeWeights();
		tracker.resample();
		tracker.meanShift();
		end=::GetTickCount();
		cout<<"time elapsed "<<end-start<<endl;

		tracker.getResult(result);
		//tracker.getMostLikely(result);
		if (i==0)
		{
			center.x=result.x+result.width/2;
			center.y=result.y+result.height/2;
		} 
		else
		{
			preCenter=center;
			center.x=(result.x+result.width/2)*(1-ALPHA_RESULT)+ALPHA_RESULT*preCenter.x;
			center.y=(result.y+result.height/2)*(1-ALPHA_RESULT)+ALPHA_RESULT*preCenter.y;
			if(abs(center.x-preCenter.x)>frame->width/100 || abs(center.y-preCenter.y)>frame->height/100){
				center=preCenter;
				moveCount++;
				if(moveCount>30)
				{
					center.x=result.x+result.width/2;
					center.y=result.y+result.height/2;
				}
			}
			else
				moveCount=0;
			//cout<<'\n'<<abs(center.x-preCenter.x)<<'\t'<<frame->width/100<<'\t'<<abs(center.y-preCenter.y)<<'\t'<<frame->height/100<<'\n';
		}

		cvRectangle( raw, cvPoint( MIN(center.x,frame->width), MIN(center.y,frame->height) ), 
			cvPoint( MIN(center.x+2,frame->width), MIN(center.y+2,frame->height) ),
			CV_RGB(0,255,0), 5, 8, 0 );

		data.seekg(0);
		data<<(int)(LimitRegion.y+LimitRegion.height-center.y)<<endl;

		cvRectangle( raw, cvPoint( MIN(result.x,frame->width), MIN(result.y,frame->height) ), 
			cvPoint( MIN(result.x+result.width,frame->width), MIN(result.y+result.height,frame->height) ),
			CV_RGB(255,0,0), 5, 8, 0 );

		fprintf( file, "%d\n",start-end );

		cvShowImage( "Video", raw );

		key = cvWaitKey( 5 );
		if(((int)'s')==key){
			cout<<"stoped, press any key to continue\n";
			cvWaitKey(0);
		}
		else if(((int)'r')==key)
		{
			cout<<"relocate\n";
			tracker.reLocate(raw);
		}
		else if(((int)'q')==key)
		{
			cout<<"quit\n";
			tracker.ReleaseHSV();
			break;
		}


		tracker.ReleaseHSV();

		i++;
	}

	//--------------end
	cvReleaseCapture( &video );

	cvReleaseImage( &raw );
	fclose( file );
	data.close();
	cvWaitKey(0);
	return 0;
};
