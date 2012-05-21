#pragma once 

#include "define.h"


/* returns a pixel value from a 32-bit floating point image */
extern inline float pixval32f(IplImage* img, int r, int c)
{
	return ( (float*)(img->imageData + img->widthStep*r) )[c];
};


/* sets a pixel value in a 64-bit floating point image */
extern inline void setpix32f(IplImage* img, int r, int c, float val)
{
	( (float*)(img->imageData + img->widthStep*r) )[c] = val;
};


/*
fatal_error() prints the error message contained in format then exits with
exit code 1.
*/
extern inline void fatal_error(string& err)
{
	cerr<<"fatal error: "<<err<<endl;
	exit(1);
};


/*
replace_extension() replaces file's extension with new_extn and returns the
result.  new_extn should not include a dot unless the new extension should
contain two dots.
*/
string replace_extension(const string& file, const string& new_extn)
{
	int lastdot;   // location of extension

	lastdot=file.find_last_of('.');

	if (lastdot!=-1)//already has an extension
	{
		return file.substr(0,lastdot)+'.'+new_extn;
	}
	else
		return file+'.'+new_extn;
};


/*
prepend_path() creates a full pathname by prepending path to file.  Returns
the full pathname.
*/
extern inline string prepend_path(const string& path, const string& file)
{
	return path+file;
};



/*
remove_path() removes the path from pathname and returns the filename with
path removed.
*/
string remove_path(const string& pathname)
{
	int lastSlash = pathname.find_last_of('/');
	if (lastSlash!=-1)
		return pathname.substr(lastSlash,pathname.length()-lastSlash);
	else
		return pathname;
};

/*
getExtension().
*/
extern inline string get_extension(const string& pathname)
{
	int lastDot = pathname.find_last_of('.');
	if (lastDot!=-1)
		return pathname.substr(lastDot,pathname.length()-lastDot);
	else
		return "";
};

/*
is_image_file() returns TRUE if file represents an image file and FALSE
otherwise.  The decision is made based on the file extention.
*/
int is_image_file(string& file)
{
	// find location of file extension
	string extension = get_extension(file);

	if( extension.empty() )
		return FALSE;

	// if file hass an image extension, return TRUE
	if( extension==".png"||
		extension==".jpg"||
		extension==".jpeg"||
		extension==".pbm"||
		extension==".pgm"||
		extension==".ppm"||
		extension==".bmp"||
		extension==".tif"||
		extension==".tiff" )
	{
		return TRUE;
	}

	// otherwise return FASLE
	return FALSE;
};


/*
draw_x() draws an X at point pt on image img.  The X has radius r, weight w,
and color c.
*/
void draw_x(IplImage* img, CvPoint pt, int r, int w, CvScalar color)
{
	cvLine( img, pt, cvPoint(pt.x+r, pt.y+r), color, w, 8, 0 );
	cvLine( img, pt, cvPoint(pt.x-r, pt.y+r), color, w, 8, 0 );
	cvLine( img, pt, cvPoint(pt.x+r, pt.y-r), color, w, 8, 0 );
	cvLine( img, pt, cvPoint(pt.x-r, pt.y-r), color, w, 8, 0 );
};
