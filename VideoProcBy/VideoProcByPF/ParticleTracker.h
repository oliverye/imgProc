#pragma once
#ifndef PARTICLETRACKER_H
#define PARTICLETRACKER_H

/******************************* Includes *********************************/
#include "HSVHistogram.h"
#include "Particle.h"
#include "Tracker.h"
/******************************* Class Definition **************************/
class ParticleTracker : public Tracker
{
private:
	int frameWidth;/**< width of frame */
	int frameHeight;/**< height of frame */
	
	CHSVHistogram orgHisto; /**< reference histogram describing region being tracked */

	vector<CParticle> particlePool;/**< particles used for tracking */
	int numParticle;/**< number of particles */

	gsl_rng* rng;

public:
	ParticleTracker(void){};

	~ParticleTracker(void){};

	void initialize(IplImage* bgrImg, int numParticle);

	void transition();

	void updateWeights();

	void normalizeWeights();

	void resample();

	virtual void getResult(CvRect& rect);

	int getParticleNum(){return numParticle;};
	/*
	get particle

	@param i index of particle
	*/
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
};
#endif