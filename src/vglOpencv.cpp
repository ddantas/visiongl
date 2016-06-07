/*********************************************************************
***                                                                ***
***  Source file vglOpencv                                         ***
***                                                                ***
*********************************************************************/

#ifndef __OPENCV__

#include <vglOpencv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************************************************************
***  IplImage                                                      ***
*********************************************************************/

void cvReleaseImage(IplImage** p_ipl)
{
  IplImage* ipl = *p_ipl;
  if (!ipl)
  {
    fprintf(stdout, "%s:%s: Warning: image is null\n", __FILE__, __FUNCTION__);
    return;
  }
  free(ipl->imageDataOrigin);

  if (ipl->roi)
    free(ipl->roi);
  if (ipl->maskROI)
    free(ipl->maskROI);
  if (ipl->tileInfo)
    free(ipl->tileInfo);

  p_ipl = NULL;
}

IplImage* cvCreateImage(CvSize size, int depth, int channels)
{
  IplImage* ipl = (IplImage*) malloc(sizeof(struct _IplImage));

  size_t bytesPerPixel = depth & 255;
  bytesPerPixel /= 8;
  if (bytesPerPixel < 1)
    bytesPerPixel = 1;

  size_t widthStep = size.width * channels * bytesPerPixel;
  size_t imageSize = size.height * widthStep;

  ipl->nSize     = sizeof(IplImage);
  ipl->ID        = 0;
  ipl->nChannels = channels;
  ipl->alphaChannel = 0;
  ipl->depth     = depth;
  ipl->dataOrder = 0;
  ipl->origin    = 0;
  ipl->align     = 4;
  ipl->width     = size.width;
  ipl->height    = size.height;
  ipl->roi       = NULL;
  ipl->maskROI   = NULL;
  ipl->imageId   = NULL;
  ipl->tileInfo  = NULL;
  ipl->imageSize = imageSize;
  ipl->imageData = (char*) malloc(imageSize);
  ipl->widthStep = widthStep;
  ipl->imageDataOrigin = ipl->imageData;

  return ipl;
}

IplImage* cvCopy(IplImage* src, IplImage* dst)
{
  //IplImage* dst = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
  memcpy(dst->imageData, src->imageData, src->imageSize);
  return dst;
}

void cvCvtColor(const IplImage* src, IplImage* dst, int code)
{
  fprintf(stderr, "%s:%s: Error: please recompile using WITH_OPENCV = 1.\n", __FILE__, __FUNCTION__);
  exit(1);
}

IplImage* cvLoadImage(const char* filename, int iscolor)
{
  fprintf(stderr, "%s:%s: Error: please recompile using WITH_OPENCV = 1 or use iplLoadPgm instead.\n", __FILE__, __FUNCTION__);
  exit(1);
}

int cvSaveImage(const char* filename, const IplImage* image, const int* params)
{
  fprintf(stderr, "%s:%s: Error: please recompile using WITH_OPENCV = 1 or use iplSavePgm instead.\n", __FILE__, __FUNCTION__);
  exit(1);
}


#endif
