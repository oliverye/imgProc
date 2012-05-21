#pragma once
#ifndef PARTICLES_H
#define PARTICLES_H

class CParticle
{
public:
	int id;			  /**< for group */
	float x;          /**< current x coordinate */
	float y;          /**< current y coordinate */
	float s;          /**< scale */
	float xp;         /**< previous x coordinate */
	float yp;         /**< previous y coordinate */
	float sp;         /**< previous scale */
	float x0;         /**< original x coordinate */
	float y0;         /**< original y coordinate */
	int width;        /**< original width of region described by particle */
	int height;       /**< original height of region described by particle */
	float w;          /**< weight */

	CParticle(float x=0, float y=0, int width=0, int height=0, float scale=1, float weight=0){
		this->x=this->x0=this->xp=x;
		this->y=this->y0=this->yp=y;
		this->height=height;
		this->width=width;
		this->s=this->sp=scale;
		this->w = weight;
	};
	//~CParticle(void);

	bool operator < (CParticle& rhs){ return w<rhs.w;};
	bool operator == (CParticle& rhs){ return w<rhs.w;};
};
#endif