#pragma once
#ifndef HYBRIDTRACKER_H
#define HYBRIDTRACKER_H

#include "HSVHistogram.h"
#include "Particle.h"
#include "Tracker.h"

class HybridTracker:public Tracker
{
private:
	gsl_rng* rng;

	int frameWidth;/**< width of frame */
	int frameHeight;/**< height of frame */

	CHSVHistogram orgHisto; /**< reference histogram describing region being tracked */
	
	CHSVHistogram overlap;
	CHSVHistogram noOverlap;
	CHSVHistogram noOverlap1;
	CHSVHistogram noOverlap2;

	vector<CParticle> particlePool;/**< particles used for tracking */
	int numParticle;/**< number of particles */

	vector<double> weight;

	void multiThread(CHSVHistogram& noOverlap,int start, int end, CvRect& overlapRegion);
public:

	HybridTracker(){};
	~HybridTracker(void){};

	void initialize(IplImage* bgrImg, int numParticle);

	void transition();

	void updateWeights();
	void updateWeightsV2();
	void updateWeightsV3();

	void normalizeWeights();

	void resample();

	void meanShift();

	void reLocate(IplImage* bgr);

	virtual void getResult(CvRect& rect);

	void getMostLikely(CvRect& result);

	const CParticle* getParticle(int i ){

		if(i>=numParticle || i<0)
			return NULL;
		return &(particlePool[i]);
	};

	void printParticle(int i){

		if(i>=numParticle || i<0)
			return;

		cout<<"Particle "<<i<<" :\n"<<"x: "<<particlePool[i].x<<"\ty: "<<particlePool[i].y;
		cout<<"\tscale: "<<particlePool[i].s<<"\tweight: "<<particlePool[i].w;
		cout<<"\twidth: "<<particlePool[i].width<<"\theight: "<<particlePool[i].height<<endl;
	};

	void printAllParticle(){
		cout<<"----------------------------------------\n";
		for (int i=0;i<numParticle;i++)
		{
			printParticle(i);
		}
		cout<<"----------------------------------------\n";
	};

	int getParticleNum(){return numParticle;};
};
#endif
