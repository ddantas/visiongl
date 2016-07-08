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

//uint8_t, int16_t etc
#include <stdint.h>

//toupper, tolower
#include <ctype.h>

#include <vglTiffIo.h>


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

void cvCvtColor(IplImage* src, IplImage* dst, int code)
{

  int b = src->depth / 8;
  if (b < 1) b = 1; //b is the size in bytes of a pixel with the depth color format
  int w = src->width;
  int h = src->height;
  int d = src->depth;
  int nPixels = w * h;
  int datasize;

  switch (code)
  {
    case CV_BGR2BGRA:
    //case CV_RGB2RGBA:
    {
      if (src->nChannels == 4)
      {
        fprintf(stdout, "%s:%s: Warning: image already has 4 channels\n", __FILE__, __FUNCTION__);
        return;
      }
      else if (src->nChannels != 3)
      {
        fprintf(stderr, "%s:%s: Error: image should have 3 channels but has %d channels\n", __FILE__, __FUNCTION__, src->nChannels);
        return;
      }
      if (3*src->width != src->widthStep)
      {
        fprintf(stderr, "%s:%s: Error: Padded image not supported, or 3*%d != %d. Please recompile using WITH_OPENCV = 1.\n", __FILE__, __FUNCTION__, src->width, src->widthStep);
        return;
      }
      dst = cvCreateImage(cvSize(w, h), d, 4);

      int datasize = nPixels * 4 * b;
      int iSrc = 0;
      uint8_t temp_alpha = 0;

      for(int iDst = 0; iDst < (datasize/d); iDst++)//for(int i = (datasize/d)-1; i >= 0; i--)
      {
        if (((iDst+1) % 4) == 0)
        {
          switch(d)
          {
            case 1:
              ((uint8_t*)dst->imageData)[iDst] = temp_alpha;
              break;
            case 2:
              ((uint16_t*)dst->imageData)[iDst] = temp_alpha;
              break;
            case 4:
              ((uint32_t*)dst->imageData)[iDst] = temp_alpha;
              break;
            case 8:
              ((uint64_t*)dst->imageData)[iDst] = temp_alpha;
              break;
          }
        }
        else
        {
          switch(d)
          {
            case 1:
              ((uint8_t*)dst->imageData)[iDst] = ((uint8_t*)src->imageData)[iSrc];
              break;
            case 2:
              ((uint16_t*)dst->imageData)[iDst] = ((uint16_t*)src->imageData)[iSrc];
              break;
            case 4:
              ((uint32_t*)dst->imageData)[iDst] = ((uint32_t*)src->imageData)[iSrc];
              break;
            case 8:
              ((uint64_t*)dst->imageData)[iDst] = ((uint64_t*)src->imageData)[iSrc];
              break;
          }
          iSrc++;
        }
      }
      break;
    }
    //case CV_BGRA2BGR:
    //case CV_RGBA2RGB:


      break;
    default:
      fprintf(stderr, "%s:%s: Error: Code %d not implemented. Please recompile using WITH_OPENCV = 1.\n", __FILE__, __FUNCTION__, code);
      exit(1);
  }
}


/** Return file extension from file name.

 */
char* getFileExtensionUppercase(const char* filename)
{
  char* ext = (char*) malloc(32);
  const int len = 4;
  if (strlen(filename) > len)
  {
    sprintf(ext, "%s", filename + strlen(filename) - len); //TODO: Implement saving by extension.
    printf("Filename: %s\n", filename);
    printf("Extension: %s\n", ext);
    for (int i = 0; i < len; i++)
    {
      ext[i] = toupper(ext[i]);
    }
    printf("Extension: %s\n", ext);
    return ext;
  }
  return NULL;
}


IplImage* cvLoadImage(char* filename, int iscolor /*= CV_LOAD_IMAGE_UNCHANGED*/)
{
  IplImage* iplImage;

  char* ext = getFileExtensionUppercase(filename);
  if ( strcmp(ext, ".PGM") || strcmp(ext, ".PPM") )
  {
    iplImage = iplLoadPgm((char*) filename);
  }
  else if ( strcmp(ext, "TIFF") || strcmp(ext, ".TIF") )
  {
#ifdef __TIFF__
    iplImage = iplLoadTiff((char*) filename);
    if (iplImage)
    {
      return (iplImage);
    }
    else
    {
      fprintf(stderr, "%s:%s: Error loading image from file %s.\n", __FILE__, __FUNCTION__, filename);
      exit(1);
    }
#else
    fprintf(stderr, "%s:%s: Error: TIFF format unsupported. Please recompile using WITH_TIFF = 1.\n", __FILE__, __FUNCTION__);
    exit(1);
#endif
  }

  fprintf(stderr, "%s:%s: Error: extension %s unsupported. You may try to recompile using WITH_OPENCV = 1 or use iplLoadPgm instead.\n", __FILE__, __FUNCTION__, ext);
  exit(1);
}

int cvSaveImage(char* filename, IplImage* image, int* params /*=0*/)
{
  IplImage* iplImage;

  char* ext = getFileExtensionUppercase(filename);
  if ( strcmp(ext, ".PGM") || strcmp(ext, ".PPM") )
  {
    return iplSavePgm((char*) filename, (IplImage*) image);
  }

  else if ( strcmp(ext, "TIFF") || strcmp(ext, ".TIF") )
  {
#ifdef __TIFF__
    iplSaveTiff(image, (char*) filename);
#else
    fprintf(stderr, "%s:%s: Error: TIFF format unsupported. Please recompile using WITH_TIFF = 1.\n", __FILE__, __FUNCTION__);
    exit(1);
#endif
  }

  fprintf(stderr, "%s:%s: Error: extension %s unsupported. You may try to recompile using WITH_OPENCV = 1 or use iplSavePgm instead.\n", __FILE__, __FUNCTION__, ext);
  exit(1);
}


#endif
