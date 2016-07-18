/*********************************************************************
***                                                                ***
***  Source file iplImage                                          ***
***                                                                ***
*********************************************************************/

#ifndef __OPENCV__

#include <vglOpencv.h>
#include <iplImage.h>



void cvReleaseImage(IplImage** p_ipl)
{
  iplReleaseImage(p_ipl);
}

IplImage* cvCreateImage(CvSize size, int depth, int channels)
{
  return iplCreateImage(size, depth, channels);
}

IplImage* cvCopy(IplImage* src, IplImage* dst)
{
  return iplCopy(src, dst);
}

void cvCvtColor(IplImage* src, IplImage* dst, int code)
{
  iplCvtColor(src, dst, code);
}

IplImage* cvLoadImage(char* filename, int iscolor /*= CV_LOAD_IMAGE_UNCHANGED*/)
{
  return iplLoadImage(filename, iscolor);
}

int cvSaveImage(char* filename, IplImage* image, int* params /*= 0*/)
{
  return iplSaveImage(filename, image, params);
}


#endif
