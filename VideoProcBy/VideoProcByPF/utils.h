#pragma once
/*
1：不能再头文件中定义函数，否则出现LNK2005及LNK1169 符号重定义错误
	另已解决办法是：
	在项目->属性->链接器->命令行->附加选项中加 /force   

	/force 表示内联的意思；但这种方法不怎么好

2：inline在cpp和h文件中都要声明为extern，否则在其他文件引用函数是会报错“找不到定义”

以上两点详细说明见 错误/inline及C++中不再类中定义函数的方法
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
