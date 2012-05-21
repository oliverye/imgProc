#pragma once
#include "HSVHistogram.h"

CHSVHistogram::CHSVHistogram(IplImage* frame, CvRect& region){
	CalculateHisto(frame,region,true);
};
void CHSVHistogram::plus(CHSVHistogram& rhs){
	for(int i=0;i<histo.size();i++)
		histo[i]+=rhs[i];
};
void CHSVHistogram::minus(CHSVHistogram& rhs){
	for(int i=0;i<histo.size();i++)
		histo[i]-=rhs[i];
};
void CHSVHistogram::CalculateHisto(IplImage* frame, CvRect& region,bool IsNormalize){

	int r, c, bin;

	if(this->histo.size()!=NH*NS + NV)
		this->histo.resize(NH*NS + NV,0);

	CvScalar scalar;
	int binTest=0;

	/* increment appropriate histogram bin for each pixel */
	for( r = region.y; r < region.y+region.height; r++ )
		for( c = region.x; c < region.x+region.width; c++ )
		{
			if(r<frame->height && c<frame->width && r>=0 && c>=0)
			{
				scalar=cvGet2D(frame,r,c);
				binTest=histoBin( scalar.val[0],scalar.val[1],scalar.val[2] );
				this->histo[binTest] += 1;
			}
		}
	if(IsNormalize==true)
		this->normalize();
};

void CHSVHistogram::CalculateHisto(IplImage* frame, CvRect& innerRegion, CvRect& outerRegion,bool IsNormalize)
{
	int r, c, bin;

	if(this->histo.size()!=NH*NS + NV)
		this->histo.resize(NH*NS + NV,0);

	CvScalar scalar;
	int binTest=0;

	int count=0;
	/* increment appropriate histogram bin for each pixel */
	for( r = outerRegion.y; r < outerRegion.y+outerRegion.height; r++ )
		for( c = outerRegion.x; c < outerRegion.x+outerRegion.width; c++ )
		{
			if(r<frame->height && c<frame->width && r>=0 && c>=0 &&
				!(r>innerRegion.y && c>innerRegion.x && c<=(innerRegion.x+innerRegion.width) && r<=(innerRegion.y+innerRegion.height)))
			{
				scalar=cvGet2D(frame,r,c);
				binTest=histoBin( scalar.val[0],scalar.val[1],scalar.val[2] );
				this->histo[binTest] += 1;
				count++;
			}
		}
	//cout<<count<<'\t'<<outerRegion.height*outerRegion.width<<endl;
	if(IsNormalize==true)
		this->normalize();
};
int CHSVHistogram::histoBin( float h, float s, float v ){
	
	int hd, sd, vd;

	/* if S or V is less than its threshold, return a "colorless" bin */
	vd = MIN( (int)(v * NV / V_MAX), NV-1 );
	if( s < S_THRESH  ||  v < V_THRESH )
		return NH * NS + vd;

	/* otherwise determine "colorful" bin */
	hd = MIN( (int)(h * NH / H_MAX), NH-1 );
	sd = MIN( (int)(s * NS / S_MAX), NS-1 );
	return sd * NH + hd;
};

void CHSVHistogram::normalize(){
	
	float sum=0;
	int i;
	for ( i=0;i<histo.size();i++)
	{
		if(i<100 && i>=80)
			histo[i]*=40;//over weight on black color
		sum+=histo.at(i);
	}
	for ( i=0;i<histo.size();i++)
		histo[i]/=sum;
};

float CHSVHistogram::distance( CHSVHistogram& h1 ){
	
	float sum = 0;
	int i;

	/*
	According the the Battacharyya similarity coefficient,

	D = \sqrt{ 1 - \sum_1^n{ \sqrt{ h_1(i) * h_2(i) } } }
	*/
	for( i = 0; i < histo.size(); i++ )
		sum += sqrt( histo[i]*h1[i] );
	return 1.0 - sum;
};

float CHSVHistogram::likehood(IplImage* img, int y, int x, int w, int h){
	
	float d_sq;

	CHSVHistogram histoNew;
	histoNew.CalculateHisto(img,cvRect(x,y,w,h),true);

	/* compute likelihood as e^{\lambda D^2(h, h^*)} */
	d_sq = this->distance(histoNew);

	return exp( -LAMBDA * d_sq );
};
float CHSVHistogram::likehood(CHSVHistogram& rhs){
	return exp( -LAMBDA * this->distance(rhs) );
};
int CHSVHistogram::exportData( string filename ){
	
	int i;
	FILE* file = fopen( filename.data(), "w" );

	if( ! file )
		return 0;

	for( i = 0; i < histo.size(); i++ )
		fprintf( file, "%d %f\n", i, histo.at(i) );
	fclose( file );
	return 1;
};
void CHSVHistogram::filter(CHSVHistogram& newTemp, double alpha)
{
	if(newTemp.length()!=histo.size())
		return;

	for (int i=0;i<histo.size();i++)
	{
		histo[i]=histo[i]*(1-alpha)+newTemp[i]*alpha;
	}
};