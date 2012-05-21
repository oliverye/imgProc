#pragma once
/*
1��������ͷ�ļ��ж��庯�����������LNK2005��LNK1169 �����ض������
	���ѽ���취�ǣ�
	����Ŀ->����->������->������->����ѡ���м� /force   

	/force ��ʾ��������˼�������ַ�������ô��

2��inline��cpp��h�ļ��ж�Ҫ����Ϊextern�������������ļ����ú����ǻᱨ���Ҳ������塱

����������ϸ˵���� ����/inline��C++�в������ж��庯���ķ���
*/

#include "define.h"

extern inline float pixval32f(IplImage* img, int r, int c);
extern inline void setpix32f(IplImage* img, int r, int c, float val);
extern inline void fatal_error(string& err);
extern inline string prepend_path(const string& path, const string& file);
extern inline string get_extension(const string& pathname);

string replace_extension(const string& file, const string& new_extn);
string remove_path(const string& pathname);
int is_image_file(string& file);
void draw_x(IplImage* img, CvPoint pt, int r, int w, CvScalar color);
