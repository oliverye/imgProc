#pragma once
#include "define.h"
#include "HSVHistogram.h"
#include "Tracker.h"

class MeanShiftTracker:public Tracker
{
public:
	MeanShiftTracker(void){};
	~MeanShiftTracker(void){};
	void initialize(IplImage* frame);
	double goToNextLocation();
	virtual void getResult(CvRect& rect);
private:
	int frameWidth;/**< width of frame */
	int frameHeight;/**< height of frame */
	CHSVHistogram orgHisto;
	vector<double> weight;
};
