
#include "Detector.h"

CDetector::CDetector(void)
{
}

CDetector::~CDetector(void)
{
}
float CDetector::data(IplImage* img, int row, int col){

	return img->imageData + col + row*img->widthStep;
};

vector<CvRect*> CDetector::detect(IplImage* img){

	int row=0,col=0;

};