#include "HSVHistogram.h"
#include "cv.h"
#include "cxcore.h"

void TestConstructor(){

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

	CHSVHistogram a(img32,region);

	for (int i=0;i<a.length();i++)
	{
		cout<<a[i]<<endl;
	}

	cvReleaseImage(&img);
	cvReleaseImage(&img32);
}
void TestLikehood(){

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

	CHSVHistogram a(img32,region);

	cout<<a.likehood(img32,img32->height/4,img32->width/4,img32->height/2,img32->width/2)<<endl;
	
	//should be much less than former output
	cout<<a.likehood(img32,img32->height-img32->height/4,img32->width-img32->width/4,img32->height/2,img32->width/2)<<endl;
	
	cvReleaseImage(&img);
	cvReleaseImage(&img32);
}
void TestExport(){

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

	CHSVHistogram a(img32,region);

	fileName="D:\\programe\\C++\\Mil\\VideoProcByPF\\histo.txt";
	cout<<a.exportData(fileName);

	cvReleaseImage(&img);
	cvReleaseImage(&img32);
}