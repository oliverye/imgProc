#pragma once

class CDetector
{
public:
	CDetector(void);
	~CDetector(void);

	//��ȡԴ����
	static float data(IplImage* img, int row, int col);
	
	//��������������������ͼƬ
	static vector<CvRect*> detect(IplImage* img);
};
