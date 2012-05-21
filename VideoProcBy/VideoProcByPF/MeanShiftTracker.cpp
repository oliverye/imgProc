#include "MeanShiftTracker.h"

void MeanShiftTracker::initialize(IplImage* bgrImg)
{
	getTrackingObject(bgrImg);

	ReadAndChangeToHSV(bgrImg);

	orgHisto.CalculateHisto(HSVImg,region,true);
	weight.resize(NH*NS + NV,0);

	frameWidth = bgrImg->width;
	frameHeight = bgrImg->height;

	ReleaseHSV();
};
double MeanShiftTracker::goToNextLocation()
{
	//calculate histogram
	CHSVHistogram currHisto;
	currHisto.CalculateHisto(HSVImg,region,true);

	//update weight
	double sumWeight=0;
	double sumX=0;
	double sumY=0;
	for (int i=0;i<weight.size();i++)
	{
		if(currHisto[i]!=0)
			weight[i]=sqrt(orgHisto.at(i)/currHisto.at(i));
		else
			weight[i]=0;
	}

	//calculate meanshift vector

	int bin=0;
	CvScalar s;

	for( int r = region.y; r < region.y+region.height; r++ )
		for( int c = region.x; c < region.x+region.width; c++ )
		{
			if(r<HSVImg->height && c<HSVImg->width && r>=0 && c>=0)
			{
				s = cvGet2D(HSVImg, r, c);
				bin = currHisto.histoBin( s.val[0],s.val[1],s.val[2] );
				sumX+=weight[bin]*c;
				sumY+=weight[bin]*r;
				sumWeight+=weight[bin];
			}
		}

	double tmpX=sumX/sumWeight;
	double tmpY=sumY/sumWeight;

	double distance = sqrt(pow(tmpX-region.x-region.width/2,2)+pow(tmpY-region.y-region.height/2,2));

	//meanshift 计算的是中心点位置，opencv里存的是左上角点的位置
	region.x=MIN(MAX(0,tmpX-region.width/2),HSVImg->width);
	region.y=MIN(MAX(0,tmpY-region.height/2),HSVImg->height);

	return distance;
};

void MeanShiftTracker::getResult(CvRect& rect){
	rect.x=region.x;
	rect.y=region.y;
	rect.width=region.width;
	rect.height=region.height;
};
