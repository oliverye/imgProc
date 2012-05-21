#include <Windows.h>
#include "FeatureTracker.h"
using namespace std;
int Thresh=130;
void onTrackerSlid(int th){
	Thresh=th;
}

int mainForPicFeaturePoint()
{
	CvCapture* video=NULL;
	IplImage* frame=NULL;
	IplImage *raw=NULL;
	string fileName="../real2.avi";

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

	//frame = cvLoadImage("../level.jpg");
	CvSize dstSize;
	dstSize = cvGetSize(frame);
	dstSize.height*=SCALE;
	dstSize.width*=SCALE;

	raw=cvCreateImage(dstSize,frame->depth,frame->nChannels);
	cvResize(frame,raw);

	cvNamedWindow( "Video", 1 );
	DWORD start=0,end=0;
	int i=0;
	int key;

	CvPoint center;

	FILE* file = fopen( "../recordFeature.txt", "w" );
	if( ! file )
		return 0;

	fstream data;
	data.open("../data.txt",ios::trunc|ios::in|ios::out,0x20);

	FeatureTracker tracker;
	tracker.initialize(raw);

	//CvMemStorage* storage = cvCreateMemStorage(0);

	cout<<"\ninput a integer(0-255) for threshold, 130 recommended\n";
	cin>>Thresh;
	cvCreateTrackbar("Threshold", "Video", &Thresh, 255, onTrackerSlid);

	//逐帧读取视频
	while(frame = cvQueryFrame( video ))//cvLoadImage("../level.jpg"))
	{
		i++;

		cvResize(frame,raw);

		tracker.getPointPic(raw,center,Thresh);
		//tracker.getPoint(raw,center);

		cvCircle(raw,center,3,CV_RGB(255,0,0),-1);
		cvLine( raw, cvPoint(0,tracker.getMidPosi()),
			cvPoint(tracker.getRegion().x+tracker.getRegion().width,tracker.getMidPosi()),
			CV_RGB(255,0,0), 1, CV_AA, 0 );

		//cvRectangle( raw, cvPoint( tracker.getLimitRegion().x, tracker.getLimitRegion().y) , 
		//		cvPoint( tracker.getLimitRegion().x+tracker.getLimitRegion().width, 
		//		tracker.getLimitRegion().y+tracker.getLimitRegion().height ),
		//		CV_RGB(0,255,0), 5, 8, 0 );

		data.seekg(0);
		data<<(int)(tracker.getMidPosi()-center.y)<<endl;

		cvShowImage("Video",raw);
		cvResetImageROI( raw );

		key = cvWaitKey( 5 );
		if(((int)'s')==key){
			cout<<"stoped, press any key to continue\n";
			cvWaitKey(0);
		}
		else if(((int)'r')==key)
		{
			cout<<"relocate\n";
			tracker.relocate(raw);
		}
		else if(((int)'q')==key)
		{
			cout<<"quit\n";
			break;
		}	
		//cvReleaseImage( &frame );
	}

	cvReleaseImage( &frame );
	cvReleaseImage( &raw );
	//cvReleaseMemStorage(&storage);
	fclose( file );
	data.close();
	cvWaitKey(0);
	return 0;
};



