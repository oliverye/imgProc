#pragma once 

#ifndef DEFS_H
#define DEFS_H

#define _WIN32_WINNT 0x0501
/********************************* Includes **********************************/

/* From standard C library */
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <algorithm>
#include <Windows.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
using namespace std;

/* From OpenCV library */
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

/* From GSL */
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


/******************************* Defs and macros *****************************/
#define SCALE 1
#ifndef MIN
#define MIN(x,y) ( ( x < y )? x : y )
#endif
#ifndef MAX
#define MAX(x,y) ( ( x > y )? x : y )
#endif
#ifndef ABS
#define ABS(x) ( ( x < 0 )? -x : x )
#endif

#define snprintf _snprintf
/******************************* Definitions for CRegion *********************************/

/* standard deviations for gaussian sampling in transition model */
#define TRANS_X_STD 1
#define TRANS_Y_STD 0.5
#define TRANS_S_STD 0.001

/* autoregressive dynamics parameters for transition model */
#define A1  2
#define A2  -1.0
#define B0  1.0

/******************************* Definitions for CHSVHistogram *********************************/

/* number of bins of HSV in histogram */
#define NH 10
#define NS 10
#define NV 10

/* max HSV values */
#define H_MAX 360.0
#define S_MAX 1.0
#define V_MAX 1.0

/* low thresholds on saturation and value for histogramming */
#define S_THRESH 0.1
#define V_THRESH 0.2

/* distribution parameter */
#define LAMBDA 10

/******************************** Definitions for main********************************/

/* command line options */
#define OPTIONS ":p:oah"

/* default number of particles */
#define PARTICLES_FOR_PF 120
#define PARTICLES_FOR_HYBRID 50
//30 for hybrid
//100 for PF

/* default basename and extension of exported frames */
#define EXPORT_BASE "./frames/frame_"
#define EXPORT_EXTN ".png"


/* maximum number of objects to be tracked */
//CURRENTLY ONLY SUPPORT 1
#define MAX_OBJECTS 2
/******************************** Definitions for template update********************************/
#define  ALPHA 0.01

/******************************** Definitions for result update********************************/
#define  ALPHA_RESULT 0.7
//result*(1-ALPHA_RESULT)+preResult*ALPHA_RESULT;
/******************************** Definitions for Meanshift********************************/
#define  MAX_ITER 20
#define  STOP_CRITERIA 5

#endif