#include "Region.h"
#include "cv.h"
#include "cxcore.h"

void TestInitialization(){

	char* fileName="D:\\programe\\C++\\Mil\\VideoProcByPF\\11.JPG";

	IplImage* img = cvLoadImage(fileName,CV_LOAD_IMAGE_ANYCOLOR |CV_LOAD_IMAGE_ANYDEPTH);

	if(img==NULL)
		return;

	IplImage* img32 = cvCreateImage( cvGetSize( img ), IPL_DEPTH_32F, 3 );
	cvScale(img,img32);
	CvRect region;
	region.x=0;
	region.y=0;
	region.width=img->width/2;
	region.height=img->height/2;

	ParticleTracker a(img32, region.x , region.y , region.width , region.height );

	for (int i=0;i<a.getRefHisto()->length();i++)
	{
		cout<<a.getRefHisto()->at(i)<<endl;
	}

	cvWaitKey(0);
	
	a.initPaticleDistribution(20);
	for (int i=0;i<a.getParticleNum();i++)
	{
		a.printParticle(i);
	}

};
void TestAlgorithm(){

	char* fileName="D:\\programe\\C++\\Mil\\VideoProcByPF\\11.JPG";

	IplImage* img = cvLoadImage(fileName,CV_LOAD_IMAGE_ANYCOLOR |CV_LOAD_IMAGE_ANYDEPTH);

	if(img==NULL)
		return;

	gsl_rng* rng;
	gsl_rng_env_setup();
	rng = gsl_rng_alloc( gsl_rng_mt19937 );
	gsl_rng_set( rng, time(NULL) );

	IplImage* img32 = cvCreateImage( cvGetSize( img ), IPL_DEPTH_32F, 3 );
	cvScale(img,img32);
	CvRect region;
	region.x=0;
	region.y=0;
	region.width=img->width/2;
	region.height=img->height/2;

	ParticleTracker a(img32, region.x , region.y , region.width , region.height );

	a.initPaticleDistribution(20);
	a.printAllParticle();
	cvWaitKey(0);

	a.transition(rng);
	a.printAllParticle();
	cvWaitKey(0);

	a.updateWeights(img32);
	a.printAllParticle();
	cvWaitKey(0);

	a.normalizeWeights();
	a.printAllParticle();
	cvWaitKey(0);

	a.resample();
	a.printAllParticle();
	cvWaitKey(0);
};
