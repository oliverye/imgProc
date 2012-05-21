#include "HybridTracker.h"
#include <Windows.h>

void HybridTracker::initialize(IplImage* bgrImg, int numParticle)
{
	gsl_rng_env_setup();
	rng = gsl_rng_alloc( gsl_rng_mt19937 );
	gsl_rng_set( rng, time(NULL) );

	getTrackingObject(bgrImg);

	getLimit(bgrImg);

	ReadAndChangeToHSV(bgrImg);

	orgHisto.CalculateHisto(HSVImg,region,true);

	this->numParticle = numParticle;
	particlePool.resize(numParticle,CParticle(region.x,region.y,region.width,region.height,1,0));

	weight.resize(NH*NS + NV,0);

	frameWidth = bgrImg->width;
	frameHeight = bgrImg->height;

	ReleaseHSV();
};

void HybridTracker::transition(){
	float x, y, s;

	for (int i=0;i<numParticle;i++)
	{
		particlePool[i].w = 0;

		/* sample new state using second-order autoregressive dynamics */
		x = A1 * ( particlePool[i].x - particlePool[i].x0 ) + A2 * ( particlePool[i].xp - particlePool[i].x0 ) +
			B0 * gsl_ran_gaussian( rng, TRANS_X_STD ) + particlePool[i].x0;
		particlePool[i].xp = particlePool[i].x;
		//if(particlePool[i].xp-x < particlePool[i].width/8)
			//particlePool[i].x = MAX( 0.0, MIN( (float)frameWidth - 1.0, x ) );
		particlePool[i].x = MAX( limit.x, MIN( limit.x+limit.width, x ) );

		y = A1 * ( particlePool[i].y - particlePool[i].y0 ) + A2 * ( particlePool[i].yp - particlePool[i].y0 ) +
			B0 * gsl_ran_gaussian( rng, TRANS_Y_STD ) + particlePool[i].y0;
		particlePool[i].yp = particlePool[i].y;
		//if(particlePool[i].yp-y < particlePool[i].width/8)
			//particlePool[i].y = MAX( 0.0, MIN( (float)frameHeight- 1.0, y ) );
		particlePool[i].y = MAX( limit.y, MIN( limit.y+limit.height, y ) );

		s = A1 * ( particlePool[i].s - 1.0 ) + A2 * ( particlePool[i].sp - 1.0 ) +
			B0 * gsl_ran_gaussian( rng, TRANS_S_STD ) + 1.0;
		particlePool[i].sp = particlePool[i].s;
		//s=MAX( 0.8, MIN(1.2,s) );
		//if((particlePool[i].x+particlePool[i].width*s) >(limit.x+limit.width))
		//	s=(limit.x+limit.width-particlePool[i].x)/(particlePool[i].width);
		//else if((particlePool[i].y+particlePool[i].height*s) >(limit.y+limit.height))
		//	s=(limit.y+limit.height-particlePool[i].y)/(particlePool[i].height);
		particlePool[i].s = MAX( 0.8, MIN(1.6,s) );//1;
	}
};

void HybridTracker::updateWeights(){
	int start,end;
	start=::GetTickCount();
	for (int i =0;i< numParticle; i++)
	{
		particlePool[i].w = orgHisto.likehood(HSVImg,particlePool[i].y, particlePool[i].x, 
			particlePool[i].width * particlePool[i].s,
			particlePool[i].height * particlePool[i].s);
	}
	end=::GetTickCount();
	cout<<"V1 time elapsed "<<end-start<<endl;

};

void HybridTracker::updateWeightsV2(){
	int x0,y0,x1,y1;
	x0=y0=0;
	x1=y1=INT_MAX;
	
	for (int i=0;i<numParticle;i++)
	{
		if(particlePool[i].x>x0)
			x0=particlePool[i].x;
		if(particlePool[i].y>y0)
			y0=particlePool[i].y;
		if(particlePool[i].x+particlePool[i].s*particlePool[i].width<x1)
			x1=particlePool[i].x+particlePool[i].s*particlePool[i].width;
		if(particlePool[i].y+particlePool[i].s*particlePool[i].height<y1)
			y1=particlePool[i].y+particlePool[i].s*particlePool[i].height;
	}

	if(x1>x0 && y1>y0)
	{
		overlap.CalculateHisto(HSVImg,cvRect(x0,y0,x1-x0,y1-y0),false);

		for (int i =0;i< numParticle; i++)
		{
			noOverlap.CalculateHisto(HSVImg,cvRect(x0,y0,x1-x0,y1-y0),
				cvRect(particlePool[i].x, particlePool[i].y, 
				particlePool[i].width * particlePool[i].s,
				particlePool[i].height * particlePool[i].s),
				false);

			double sum=0;
			double norSum=0;
			for (int i=0;i<orgHisto.length();i++)
			{
				sum+=sqrt(orgHisto[i]*(overlap[i]+noOverlap[i]));
				norSum+=overlap[i]+noOverlap[i];
				noOverlap.set(i,0);
			}

			particlePool[i].w = exp( -LAMBDA * (1-sum/sqrt(norSum)) );
		}	
	}
	else
	{
		for (int i =0;i< numParticle; i++)
		{
			particlePool[i].w = orgHisto.likehood(HSVImg,particlePool[i].y, particlePool[i].x, 
				particlePool[i].width * particlePool[i].s,
				particlePool[i].height * particlePool[i].s);
		}
	}
	
};
void HybridTracker::updateWeightsV3(){
	int x0,y0,x1,y1;
	x0=y0=0;
	x1=y1=INT_MAX;

	for (int i=0;i<numParticle;i++)
	{
		if(particlePool[i].x>x0)
			x0=particlePool[i].x;
		if(particlePool[i].y>y0)
			y0=particlePool[i].y;
		if(particlePool[i].x+particlePool[i].s*particlePool[i].width<x1)
			x1=particlePool[i].x+particlePool[i].s*particlePool[i].width;
		if(particlePool[i].y+particlePool[i].s*particlePool[i].height<y1)
			y1=particlePool[i].y+particlePool[i].s*particlePool[i].height;
	}

	if(x1>x0 && y1>y0)
	{
		overlap.CalculateHisto(HSVImg,cvRect(x0,y0,x1-x0,y1-y0),false);
		boost::thread thrd1( boost::bind(&HybridTracker::multiThread,this,noOverlap1,0,numParticle/2,cvRect(x0,y0,x1-x0,y1-y0)));
		boost::thread thrd2( boost::bind(&HybridTracker::multiThread,this,noOverlap2,numParticle/2,numParticle,cvRect(x0,y0,x1-x0,y1-y0)));
		thrd2.join();
		thrd1.join();
	}
	else
	{
		for (int i =0;i< numParticle; i++)
		{
			particlePool[i].w = orgHisto.likehood(HSVImg,particlePool[i].y, particlePool[i].x, 
				particlePool[i].width * particlePool[i].s,
				particlePool[i].height * particlePool[i].s);
		}
	}

};
void HybridTracker::multiThread(CHSVHistogram& noOverlap,int start, int end, CvRect& overlapRegion)
{
	for (int i =start;i< end; i++){
		noOverlap.CalculateHisto(HSVImg,overlapRegion,
			cvRect(particlePool[i].x, particlePool[i].y, 
			particlePool[i].width * particlePool[i].s,
			particlePool[i].height * particlePool[i].s),
			false);

		double sum=0;
		double norSum=0;
		for (int i=0;i<orgHisto.length();i++)
		{
			sum+=sqrt(orgHisto[i]*(overlap[i]+noOverlap[i]));
			norSum+=overlap[i]+noOverlap[i];
			noOverlap.set(i,0);
		}

		particlePool[i].w = exp( -LAMBDA * (1-sum/sqrt(norSum)) );
		//cout<<"weight\t"<<particlePool[i].w;
	}
};
void HybridTracker::normalizeWeights(){
	float sum=0;

	for (int i =0;i< numParticle; i++)
		sum+=particlePool[i].w;
	for (int i =0;i< numParticle; i++)
		particlePool[i].w/=sum;
};

void HybridTracker::resample(){
	vector<CParticle> newPool;

	//sort by ascending order
	sort(particlePool.begin(),particlePool.end());

	int np=0;
	int id=0;
	for (int i=numParticle-1;i>-1;i--)
	{
		np = particlePool[i].w*numParticle;
		for (int j=0;j<np;j++)
		{
			particlePool[i].id=id;
			newPool.push_back(particlePool[i]);
			if (newPool.size()>numParticle)
				goto exit;
		}
		id++;
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

void HybridTracker::meanShift(){

	CHSVHistogram currHisto;
	CvRect rect;
	for (int i=0;i<particlePool.size();i++)
	{
		if(i==0 || particlePool[i].id!=particlePool[i-1].id)
		{
			double distance=0;
			int iter=0;
			do{

				rect = cvRect(particlePool[i].x,particlePool[i].y,
					particlePool[i].s * particlePool[i].width,
					particlePool[i].s * particlePool[i].height);

				currHisto.CalculateHisto(HSVImg,rect,true);

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

				for( int r = particlePool[i].y; r < particlePool[i].y+particlePool[i].s *particlePool[i].height; r++ )
					for( int c = particlePool[i].x; c < particlePool[i].x+particlePool[i].s *particlePool[i].width; c++ )
					{
						if(r<HSVImg->height && c<HSVImg->width)
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

				distance = sqrt(pow(tmpX-particlePool[i].x-particlePool[i].s *particlePool[i].width/2,2)
					+pow(tmpY-particlePool[i].y-particlePool[i].s *particlePool[i].height/2,2));
				iter++;

				//meanshift 计算的是中心点位置，opencv里存的是左上角点的位置
				particlePool[i].x=MIN(MAX(0,tmpX-particlePool[i].s *particlePool[i].width/2),HSVImg->width);
				particlePool[i].y=MIN(MAX(0,tmpY-particlePool[i].s *particlePool[i].height/2),HSVImg->height);
			}
			while(distance>1 && iter<5);

			//re-calculate weight
			particlePool[i].w = exp( -LAMBDA * orgHisto.distance(currHisto));
			//cout<<iter<<endl;
		}
		else
		{
			particlePool[i].x=particlePool[i-1].x;
			particlePool[i].y=particlePool[i-1].y;
			particlePool[i].w=particlePool[i-1].w;
		}
	}
};
void HybridTracker::reLocate(IplImage* bgrImg){
	getTrackingObject(bgrImg);

	orgHisto.CalculateHisto(HSVImg,region,true);

	for(int i=0;i<this->numParticle;i++)
	{
		particlePool[i].x0=particlePool[i].xp=particlePool[i].x=region.x;
		particlePool[i].y0=particlePool[i].yp=particlePool[i].y=region.y;
		particlePool[i].height=region.height;
		particlePool[i].width=region.width;
		particlePool[i].sp=particlePool[i].s=1;
		particlePool[i].w=1/numParticle;
	}

	for (int i=0;i<weight.size();i++)
	{
		weight[i]=0;
	}
};
void HybridTracker::getResult(CvRect& result){

	double tmpHeight,tmpWidth,tmpX,tmpY;
	tmpHeight=tmpWidth=tmpX=tmpY=0;
	double sumWeight=0;

	for (int k=0;k<numParticle;k++)
	{
		tmpHeight+=particlePool[k].height * particlePool[k].w * particlePool[k].s;
		tmpWidth+=particlePool[k].width* particlePool[k].w * particlePool[k].s;
		tmpX+=particlePool[k].x* particlePool[k].w;
		tmpY+=particlePool[k].y* particlePool[k].w;
		sumWeight+=particlePool[k].w;
	}
	result.height=cvRound(tmpHeight/sumWeight);
	result.width=cvRound(tmpWidth/sumWeight);
	result.x=cvRound(tmpX/sumWeight);
	result.y=cvRound(tmpY/sumWeight);
	//cout<<sumWeight<<endl;
};
void HybridTracker::getMostLikely(CvRect& result){
	CvRect tmp;
	tmp.height=tmp.width=tmp.x=tmp.y=0;

	double tmpWeight=0;
	int index=0;
	for (int k=0;k<numParticle;k++)
	{
		if(tmpWeight<particlePool[k].w)
		{
			index=k;
		}
	}
	result.height=particlePool[index].height*particlePool[index].s;
	result.width=particlePool[index].width*particlePool[index].s;
	result.x=particlePool[index].x;
	result.y=particlePool[index].y;
};