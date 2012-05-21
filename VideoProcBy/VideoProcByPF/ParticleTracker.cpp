#pragma once
#include "ParticleTracker.h"

void ParticleTracker::initialize(IplImage* bgrImg, int numParticle){
	gsl_rng_env_setup();
	rng = gsl_rng_alloc( gsl_rng_mt19937 );
	gsl_rng_set( rng, time(NULL) );

	getTrackingObject(bgrImg);

	ReadAndChangeToHSV(bgrImg);

	orgHisto.CalculateHisto(HSVImg,region,true);

	this->numParticle = numParticle;
	particlePool.resize(numParticle,CParticle(region.x,region.y,region.width,region.height,1,0));

	frameWidth = bgrImg->width;
	frameHeight = bgrImg->height;

	ReleaseHSV();
};

void ParticleTracker::transition(){
	
	float x, y, s;

	for (int i=0;i<numParticle;i++)
	{
		particlePool[i].w = 0;

		/* sample new state using second-order autoregressive dynamics */
		x = A1 * ( particlePool[i].x - particlePool[i].x0 ) + A2 * ( particlePool[i].xp - particlePool[i].x0 ) +
			B0 * gsl_ran_gaussian( rng, TRANS_X_STD ) + particlePool[i].x0;
		particlePool[i].xp = particlePool[i].x;
		particlePool[i].x = MAX( 0.0, MIN( (float)frameWidth - 1.0, x ) );

		y = A1 * ( particlePool[i].y - particlePool[i].y0 ) + A2 * ( particlePool[i].yp - particlePool[i].y0 ) +
			B0 * gsl_ran_gaussian( rng, TRANS_Y_STD ) + particlePool[i].y0;
		particlePool[i].yp = particlePool[i].y;
		particlePool[i].y = MAX( 0.0, MIN( (float)frameHeight- 1.0, y ) );

		s = A1 * ( particlePool[i].s - 1.0 ) + A2 * ( particlePool[i].sp - 1.0 ) +
			B0 * gsl_ran_gaussian( rng, TRANS_S_STD ) + 1.0;
		particlePool[i].sp = particlePool[i].s;
		particlePool[i].s = MAX( 0.5, MIN(2,s) );
	}
};

void ParticleTracker::updateWeights(){
	
	for (int i =0;i< numParticle; i++)
	{
		particlePool[i].w = orgHisto.likehood(HSVImg,particlePool[i].y, particlePool[i].x, 
											particlePool[i].width * particlePool[i].s,
											particlePool[i].height * particlePool[i].s);
	}
};

void ParticleTracker::normalizeWeights( ){
	
	float sum=0;

	for (int i =0;i< numParticle; i++)
		sum+=particlePool[i].w;
	for (int i =0;i< numParticle; i++)
		particlePool[i].w/=sum;
};

void ParticleTracker::resample( ){

	vector<CParticle> newPool;

	//sort by ascending order
	sort(particlePool.begin(),particlePool.end());

	int np=0;
	for (int i=numParticle-1;i>-1;i--)
	{
		np = particlePool[i].w*numParticle;
		for (int j=0;j<np;j++)
		{
			newPool.push_back(particlePool[i]);
			if (newPool.size()>numParticle)
				goto exit;
		}
	}

	//if not filled completely
	if (newPool.size()<numParticle)
	{
		//find the greatest particle
		int index=0;
		float tmp=particlePool[0].w;
		for (int k=1;k<numParticle;k++)
		{
			if (tmp<particlePool[k].w)
			{
				index = k;
				tmp = particlePool[k].w;
			}
		}

		int fill=numParticle - newPool.size();
		//then assign it to remaining particls
		for (int k=0;k<fill;k++)
			newPool.push_back(particlePool[index]);
	}
exit:
	particlePool=newPool;
	return;
};

void ParticleTracker::getResult(CvRect& rect){
	
	int index=0;
	float tmp=particlePool[0].w;
	for (int k=1;k<numParticle;k++)
	{
		if (tmp<particlePool[k].w)
		{
			index = k;
			tmp = particlePool[k].w;
		}
	}
	rect.x=particlePool[index].x;
	rect.y=particlePool[index].y;
	rect.width=particlePool[index].s*particlePool[index].width;
	rect.height=particlePool[index].s*particlePool[index].height;

};