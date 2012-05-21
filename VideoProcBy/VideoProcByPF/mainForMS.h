#include "MeanShiftTracker.h"
/********************************* Structures ********************************/
int mainForMS()
{
	bool showAll = false;
	string fileName="../real1.AVI";
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

	MeanShiftTracker tracker;
	tracker.initialize(frame);

	cvNamedWindow( "Video", 1 );
	DWORD start=0,end=0;
	int i=0;
	CvRect result;

	double distance=DBL_MAX;
	int iter=0;

	bool drawProc=false;

	FILE* file = fopen( "../recordMS.txt", "w" );
	if( ! file )
		return 0;

	while (frame = cvQueryFrame( video ))
	{
		start=::GetTickCount();
		show.push_back((IplImage *)cvClone( frame ));
		distance=DBL_MAX;
		iter=0;
		
		tracker.ReadAndChangeToHSV(frame);
		do 
		{
			distance = tracker.goToNextLocation();
			iter++;
			//cout<<distance<<endl;

			if (drawProc)
			{
				tracker.getResult(result);
				cvRectangle( show[i], cvPoint( result.x, result.y )
					,cvPoint( result.x+result.width, result.y+result.height )
					, CV_RGB(255,0,0), 1, 8, 0 );
				cvShowImage( "Video", show[i] );
				cvWaitKey(0);
			}
		} while (distance>STOP_CRITERIA && iter<MAX_ITER);
		
		end=::GetTickCount();
		cout<<"time elapsed "<<end-start<<endl;

		tracker.getResult(result);

		cvRectangle( show[i], cvPoint( result.x, result.y )
			,cvPoint( result.x+result.width, result.y+result.height )
			, CV_RGB(255,0,0), 5, 8, 0 );

		cvShowImage( "Video", show[i] );

		fprintf( file, "%d\n",start-end );

		if(((int)'s')==cvWaitKey( 5 ))
			cvWaitKey(0);

		tracker.ReleaseHSV();

		i++;
	}

	//--------------end
	cvReleaseCapture( &video );

	for(int j = 0; j < show.size(); j++ )
	{
		cvReleaseImage( &(show[j]) );
	}
	fclose(file);
	cvWaitKey(0);
	return 0;
};
