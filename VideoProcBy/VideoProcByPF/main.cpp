#include "mainForPF.h"
#include "mainForMS.h"
#include "mainForHybrid.h"
#include "mainForHybridCamera.h"
#include "mainForFeaturePoint.h"
#include "mainForPicFeaturePoint.h"
int main()
{
	int algorithm =6;

	switch(algorithm){
		case 1:mainForMS();break;
		case 2:mainForPF();;break;
		case 3:mainForHybrid();break;
		case 4:mainForHybridCamera();break;
		case 5:mainForFeaturePoint();break;
		case 6:mainForPicFeaturePoint();break;
	}
	cout<<"end";
	return 0;
}