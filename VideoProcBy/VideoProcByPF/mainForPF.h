#include "ParticleTracker.h"

int mainForPF()
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

	int skip=0;
	while(skip<30){
		frame = cvQueryFrame( video );
		skip++;
	}
	
	show.push_back((IplImage *)cvClone( frame ));
	cvNamedWindow( "Video", 1 );

	ParticleTracker tracker;
	tracker.initialize(frame,PARTICLES_FOR_PF);

	CvRect result;

	int i=0;

	DWORD start=0,end=0;

	FILE* file = fopen( "../recordPF.txt", "w" );
	if( ! file )
		return 0;

	while (frame = cvQueryFrame( video ))
	{

		start=::GetTickCount();
		show.push_back((IplImage *)cvClone( frame ));	

		tracker.ReadAndChangeToHSV(frame);

		tracker.transition();

		tracker.updateWeights();

		tracker.resample();

		end=::GetTickCount();
		cout<<"time elapsed "<<end-start<<endl;

		tracker.getResult(result);

		cvRectangle( show[i], cvPoint( result.x, result.y )
			,cvPoint( result.x+result.width, result.y+result.height )
			, CV_RGB(255,0,0), 5, 8, 0 );

		fprintf( file, "%d\n",start-end );

		cvShowImage( "Video", show[i] );
		
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