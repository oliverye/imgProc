#pragma once
#ifndef HSVHISTOGRAM_H
#define HSVHISTOGRAM_H
/******************************* Includes *********************************/
#include "utils.h"
#include "define.h"

/******************************* Class Definition **************************/
class CHSVHistogram
{
private:
	vector<float> histo;   /**< histogram array */

	/*
	Calculates the histogram bin into which an HSV entry falls

	@param h Hue
	@param s Saturation
	@param v Value

	/*
	Set a Histogram of a img

	@param img a img
	*/
	void setHisto(IplImage* img);
	
public:
	CHSVHistogram(void){};


	/*
	Construct a Histogram of a sub-img specified by region
	Then normalized the histogram

	@param img a img
	@param region specified region of img
	*/
	CHSVHistogram(IplImage* img, CvRect& region);
	
	void CalculateHisto(IplImage* img, CvRect& region,bool IsNormalize);
	void CalculateHisto(IplImage* img, CvRect& innerRegion, CvRect& outerRegion,bool IsNormalize);
	
	void InitToZero(){
		this->histo.resize(NH*NS + NV,0);
	}

	void plus(CHSVHistogram& rhs);
	void minus(CHSVHistogram& rhs);
	/*
	@return Returns the bin index corresponding to the HSV color defined by
	\a h, \a s, and \a v.
	*/
	int histoBin( float h, float s, float v );

	//~CHSVHistogram(void);

	/*
	Normalizes a histogram so all bins sum to 1.0
	*/
	void normalize();

	/*
	return the length of histo
	*/
	int length() const{ return histo.size();};

	/*
	Computes squared distance metric based on the Battacharyya similarity
	coefficient between histograms.

	@param h1 CHSVHistogram; should be normalized

	@return Returns a squared distance based on the Battacharyya similarity
	coefficient between \a h1 and \a this object
	*/
	float distance( CHSVHistogram& h1 );
	
	/*
	Computes the likelihood of there being a player at a given location in
	an image

	@param img image that has been converted to HSV colorspace using bgr2hsv()
	@param r row location of center of window around which to compute likelihood
	@param c col location of center of window around which to compute likelihood
	@param w width of region over which to compute likelihood
	@param h height of region over which to compute likelihood

	histo should be normalized first 
	normalized with normalize_histogram()

	@return Returns the likelihood of there being a player at location
	(\a r, \a c) in \a img
	*/
	float likehood(IplImage* img, int r, int c, int w, int h);
	float likehood(CHSVHistogram& rhs);


	/*
	Exports histogram data to a specified file.  The file is formatted as
	follows (intended for use with gnuplot:

	0 <h_0>
	...
	<i> <h_i>
	...
	<n> <h_n>

	Where n is the number of histogram bins and h_i, i = 1..n are
	floating point bin values

	@param histo histogram to be exported
	@param filename name of file to which histogram is to be exported

	@return Returns 1 on success or 0 on failure
	*/
	int exportData( string filename );

	float operator [](int i){
		if (i>histo.size() || i<0)
			return -1;
		return histo[i];
	};

	float at(int i) const{
		if (i>histo.size() || i<0)
			return -1;
		return histo[i];
	};

	void set(int i,int setValue) {
		if (i>histo.size() || i<0)
			return;
		
		histo[i]=setValue;
	};

	void filter(CHSVHistogram& pre, double alpha);

	void setAllToZero(){
		for(int i=0;i<histo.size();i++)
			histo[i]=0;
	}
};
#endif