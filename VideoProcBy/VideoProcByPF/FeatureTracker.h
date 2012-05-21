#pragma once
#include "Tracker.h"
#include "define.h"

class FeatureTracker:public Tracker
{
public:
	FeatureTracker(void);
	~FeatureTracker(void);
	
	void initialize(IplImage* frame);
	void getPoint(IplImage* frame,CvPoint& point );
	void getPointPic(IplImage* frame,CvPoint& point,int Thresh );
	void getPointLine(IplImage* frame,CvPoint& point,CvMemStorage* storage);
	int getMidPosi(){return midPosi;};
	virtual void getResult(CvRect& rect){};
	void relocate(IplImage* frame);
private:
	CvPoint posi;
	CvPoint prePosi;
	int midPosi;
	int isFirst;

	CvPoint2D32f cornersA[100];

	IplImage* tmp;
	IplImage* eig_image;
	IplImage* tmp_image;
	
	CvPoint GetCornerPointInRect(IplImage* raw);
	int getLine( IplImage* frame );
	static void mouseLine( int event, int x, int y, int flags, void* param );
};
