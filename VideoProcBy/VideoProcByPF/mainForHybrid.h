#include "HybridTracker.h"
/********************************* Structures ********************************/
#include <Windows.h>
/********************************* Functions ********************************/
void showImage(IplImage* frame);
int mainForHybrid()
{
	bool showAll = false;
	string fileName="../real1.avi";
	CvCapture* video=NULL;
	IplImage* frame=NULL;
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

	cvNamedWindow( "Video", 1 );
	DWORD start=0,end=0;
	int i=0;

	CvRect result;
	CvPoint center;
	CvPoint preCenter;

	FILE* file = fopen( "../recordHybrid.txt", "w" );
	if( ! file )
		return 0;

	HybridTracker tracker;
	tracker.initialize(frame,PARTICLES_FOR_HYBRID);

	int key;

	while (frame = cvQueryFrame( video ))
	{
		start=::GetTickCount();
		show.push_back((IplImage *)cvClone( frame ));

		tracker.ReadAndChangeToHSV(frame);
		
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
		}
		
		cvRectangle( show[i], cvPoint( MIN(center.x,frame->width), MIN(center.y,frame->height) ), 
			cvPoint( MIN(center.x+2,frame->width), MIN(center.y+2,frame->height) ),
			CV_RGB(0,255,0), 5, 8, 0 );

		cvRectangle( show[i], cvPoint( MIN(result.x,frame->width), MIN(result.y,frame->height) ), 
			cvPoint( MIN(result.x+result.width,frame->width), MIN(result.y+result.height,frame->height) ),
			CV_RGB(255,0,0), 5, 8, 0 );

		fprintf( file, "%d\n",start-end );

		cvShowImage( "Video", show[i] );

		key = cvWaitKey( 5 );
		if(((int)'s')==key){
			cout<<"stoped, press any key to continue\n";
			cvWaitKey(0);
		}
		else if(((int)'r')==key)
		{
			cout<<"relocate\n";
			tracker.reLocate(frame);
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

	for(int j = 0; j < show.size(); j++ )
	{
		cvReleaseImage( &(show[j]) );
	}
	fclose( file );
	cvWaitKey(0);
	return 0;
};
void showImage(IplImage* frame){
cvShowImage( "Video", frame );
};