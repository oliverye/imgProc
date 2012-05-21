#pragma once

class CDetector
{
public:
	CDetector(void);
	~CDetector(void);

	//获取源数据
	static float data(IplImage* img, int row, int col);
	
	//类似区域生长法，单幅图片
	static vector<CvRect*> detect(IplImage* img);
};
