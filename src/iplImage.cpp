/*********************************************************************
***                                                                ***
***  Source file iplImage                                          ***
***                                                                ***
*********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//uint8_t, int16_t etc
#include <stdint.h>

//toupper, tolower
#include <ctype.h>

#include <vglTiffIo.h>

#ifdef __OPENCV__
  #include <opencv2/imgproc/types_c.h>
  #include <opencv2/highgui/highgui_c.h>
#else
  #include <vglOpencv.h>
#endif

#include <iplImage.h>


/*********************************************************************
***  IplImage                                                      ***
*********************************************************************/

void iplReleaseImage(IplImage** p_ipl)
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

IplImage* iplCreateImage(CvSize size, int depth, int channels)
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

IplImage* iplCopy(IplImage* src, IplImage* dst)
{
  //IplImage* dst = iplCreateImage(cvGetSize(src), src->depth, src->nChannels);
  memcpy(dst->imageData, src->imageData, src->imageSize);
  return dst;
}

void iplCvtColor(IplImage* src, IplImage* dst, int code)
{

  int w = src->width;
  int h = src->height;
  int d = src->depth && 255; // d = bits per sample
  int b = d / 8;
  if (b < 1) b = 1;          // b = bytes per sample
  int nPixels = w * h;
  int datasize;
  int srcNChan;
  int dstNChan;
  int srcIChan[4];
  int dstIChan[4];


  switch (code)
  {
    case CV_GRAY2BGR:
    //case CV_GRAY2RGB: //8
    case CV_GRAY2BGRA:
    //case CV_GRAY2RGBA: //9
    {
      if (src->nChannels != 1)
      {
        fprintf(stderr, "%s:%s: Error: src image should have 1 channel but has %d channels\n", __FILE__, __FUNCTION__, src->nChannels);
        exit(1);
      }
      break;
    }
    case CV_BGR2BGRA:
    //case CV_RGB2RGBA: //0
    case CV_BGR2RGBA:
    //case CV_RGB2BGRA: //2
    case CV_BGR2RGB:
    //case CV_RGB2BGR: //4
    case CV_BGR2GRAY: //6
    case CV_RGB2GRAY: //7
    {
      if (src->nChannels != 3)
      {
        fprintf(stderr, "%s:%s: Error: src image should have 3 channels but has %d channels\n", __FILE__, __FUNCTION__, src->nChannels);
        exit(1);
      }
      break;
    }
    case CV_BGRA2BGR:
    //case CV_RGBA2RGB: //1
    case CV_RGBA2BGR:
    //case CV_BGRA2RGB: //3
    case CV_BGRA2RGBA:
    //case CV_RGBA2BGRA: //5
    case CV_BGRA2GRAY: //10
    case CV_RGBA2GRAY: //11
    {
      if (src->nChannels != 4)
      {
        fprintf(stderr, "%s:%s: Error: src image should have 4 channels but has %d channels\n", __FILE__, __FUNCTION__, src->nChannels);
        exit(1);
      }
      break;
    }
    default:
    {
      fprintf(stdout, "%s:%s: Error: conversion option = %d not implemented.\n", __FILE__, __FUNCTION__, code);
      exit(1);
    }
  }


  if (dst)
  {
    switch (code)
    {
      case CV_BGR2BGRA:
      //case CV_RGB2RGBA: //0
      case CV_BGR2RGBA:
      //case CV_RGB2BGRA: //2
      case CV_BGRA2RGBA:
      //case CV_RGBA2BGRA: //5
      case CV_GRAY2BGRA:
      //case CV_GRAY2RGBA: //9
      {
        if (dst->nChannels != 4)
        {
          fprintf(stdout, "%s:%s: Error: dst image should have 4 channels but has %d channels\n", __FILE__, __FUNCTION__, dst->nChannels);
          exit(1);
        }
        break;
      }
      case CV_BGRA2BGR:
      //case CV_RGBA2RGB: //1
      case CV_RGBA2BGR:
      //case CV_BGRA2RGB: //3
      case CV_BGR2RGB:
      //case CV_RGB2BGR: //4
      case CV_GRAY2BGR:
      //case CV_GRAY2RGB: //8
      {
        if (dst->nChannels != 3)
        {
          fprintf(stdout, "%s:%s: Error: dst image should have 3 channels but has %d channels\n", __FILE__, __FUNCTION__, dst->nChannels);
          exit(1);
        }
        break;
      }
      case CV_BGR2GRAY: //6
      case CV_RGB2GRAY: //7
      case CV_BGRA2GRAY: //10
      case CV_RGBA2GRAY: //11
      {
        if (dst->nChannels != 1)
        {
          fprintf(stdout, "%s:%s: Error: dst image should have 1 channel but has %d channels\n", __FILE__, __FUNCTION__, dst->nChannels);
          exit(1);
        }
        break;
      }
      default:
      {
        fprintf(stdout, "%s:%s: Error: conversion option = %d not implemented.\n", __FILE__, __FUNCTION__, code);
        exit(1);
      }
    }
  }

  switch (code)
  {
    case CV_BGR2BGRA:
    //case CV_RGB2RGBA: //0
    {
      srcNChan = 3;
      dstNChan = 4;
      srcIChan[0] = 0;
      srcIChan[1] = 1;
      srcIChan[2] = 2;
      srcIChan[3] = -1; //no Alpha channel in src image.
      dstIChan[0] = 0;
      dstIChan[1] = 1;
      dstIChan[2] = 2;
      dstIChan[3] = 3;
      break;
    }
    case CV_BGRA2BGR:
    //case CV_RGBA2RGB: //1
    {
      srcNChan = 4;
      dstNChan = 3;
      srcIChan[0] = 0;
      srcIChan[1] = 1;
      srcIChan[2] = 2;
      srcIChan[3] = 3;
      dstIChan[0] = 0;
      dstIChan[1] = 1;
      dstIChan[2] = 2;
      dstIChan[3] = -1; //no Alpha channel in dst image.
      break;
    }
    case CV_BGR2RGBA:
    //case CV_RGB2BGRA: //2
    {
      srcNChan = 3;
      dstNChan = 4;
      srcIChan[0] = 0;
      srcIChan[1] = 1;
      srcIChan[2] = 2;
      srcIChan[3] = -1; //no Alpha channel in src image.
      dstIChan[0] = 2;
      dstIChan[1] = 1;
      dstIChan[2] = 0;
      dstIChan[3] = 3;
      break;
    }
    case CV_RGBA2BGR:
    //case CV_BGRA2RGB: //3
    {
      srcNChan = 4;
      dstNChan = 3;
      srcIChan[0] = 0;
      srcIChan[1] = 1;
      srcIChan[2] = 2;
      srcIChan[3] = 3;
      dstIChan[0] = 2;
      dstIChan[1] = 1;
      dstIChan[2] = 0;
      dstIChan[3] = -1; //no Alpha channel in dst image.
      break;
    }
    case CV_BGR2RGB:
    //case CV_RGB2BGR: //4
    {
      srcNChan = 3;
      dstNChan = 3;
      srcIChan[0] = 0;
      srcIChan[1] = 1;
      srcIChan[2] = 2;
      srcIChan[3] = -1; //no Alpha channel in src image.
      dstIChan[0] = 2;
      dstIChan[1] = 1;
      dstIChan[2] = 0;
      dstIChan[3] = -1; //no Alpha channel in dst image.
      break;
    }
    case CV_BGRA2RGBA:
    //case CV_RGBA2BGRA: //5
    {
      srcNChan = 4;
      dstNChan = 4;
      srcIChan[0] = 0;
      srcIChan[1] = 1;
      srcIChan[2] = 2;
      srcIChan[3] = 3;
      dstIChan[0] = 2;
      dstIChan[1] = 1;
      dstIChan[2] = 0;
      dstIChan[3] = 3; //no Alpha channel in dst image.
      break;
    }
    case CV_BGR2GRAY: //6
    {
      srcNChan = 3;
      dstNChan = 1;
      srcIChan[0] = 2;
      srcIChan[1] = 1;
      srcIChan[2] = 0;
      srcIChan[3] = -1; //no Alpha channel in src image.
      break;
    }
    case CV_RGB2GRAY: //7
    {
      srcNChan = 3;
      dstNChan = 1;
      srcIChan[0] = 0;
      srcIChan[1] = 1;
      srcIChan[2] = 2;
      srcIChan[3] = -1; //no Alpha channel in src image.
      break;
    }
    case CV_GRAY2BGR:
    //case CV_GRAY2RGB: //8
    {
      srcNChan = 1;
      dstNChan = 3;
      srcIChan[0] = 0;
      srcIChan[1] = 0;
      srcIChan[2] = 0;
      srcIChan[3] = -1;
      dstIChan[0] = 0;
      dstIChan[1] = 1;
      dstIChan[2] = 2;
      dstIChan[3] = -1; //no Alpha channel in dst image.
      break;
    }
    case CV_GRAY2BGRA:
    //case CV_GRAY2RGBA: //9
    {
      srcNChan = 1;
      dstNChan = 4;
      srcNChan = 1;
      dstNChan = 3;
      srcIChan[0] = 0;
      srcIChan[1] = 0;
      srcIChan[2] = 0;
      srcIChan[3] = -1;
      dstIChan[0] = 0;
      dstIChan[1] = 1;
      dstIChan[2] = 2;
      dstIChan[3] = 3;
      break;
    }
    case CV_BGRA2GRAY: //10
    {
      srcNChan = 4;
      dstNChan = 1;
      srcIChan[0] = 2;
      srcIChan[1] = 1;
      srcIChan[2] = 0;
      srcIChan[3] = 3;
      break;
    }
    case CV_RGBA2GRAY: //11
    {
      srcNChan = 4;
      dstNChan = 1;
      srcIChan[0] = 0;
      srcIChan[1] = 1;
      srcIChan[2] = 2;
      srcIChan[3] = 3;
      break;
    }
    default:
    {
      fprintf(stdout, "%s:%s: Error: conversion option = %d not implemented.\n", __FILE__, __FUNCTION__, code);
      exit(1);
    }
  }

  //TODO: create dst image and return pointer by reference.
  if (dst == NULL)
  {
    fprintf(stderr, "%s:%s: Error: dst image is NULL.\n", __FILE__, __FUNCTION__);
    exit(1);
    //dst = iplCreateImage(cvSize(w, h), src->depth, dstNChan);
  }

  uint8_t temp_alpha = -1;
  float wR = .30;
  float wG = .59;
  float wB = .11;

  switch (code)
  {
    case CV_BGR2BGRA:
    //case CV_RGB2RGBA: //0
    case CV_BGRA2BGR: 
    //case CV_RGBA2RGB: //1
    case CV_BGR2RGBA:
    //case CV_RGB2BGRA: //2
    case CV_RGBA2BGR:
    //case CV_BGRA2RGB: //3
    case CV_BGR2RGB:
    //case CV_RGB2BGR: //4
    case CV_BGRA2RGBA:
    //case CV_RGBA2BGRA: //5
    case CV_GRAY2BGR:
    //case CV_GRAY2RGB: //8
    case CV_GRAY2BGRA:
    //case CV_GRAY2RGBA: //9
    {
      for(int i = 0; i < h; i++)
      {
        for(int j = 0; j < w; j++)
        {
          int srcOffs = i * src->widthStep + j * srcNChan;
          int dstOffs = i * dst->widthStep + j * dstNChan;
          for(int k = 0; k < dstNChan; k++)
          {
            int srcIdx = srcOffs + srcIChan[k];
            int dstIdx = dstOffs + dstIChan[k];
            if (dstIChan[k] == 3 && srcIChan[k] == -1)
            {
              switch(d)
              {
                case 1:
                  ((uint8_t*)dst->imageData)[dstIdx] = temp_alpha;
                  break;
                case 2:
                  ((uint16_t*)dst->imageData)[dstIdx] = temp_alpha;
                  break;
                case 4:
                  ((uint32_t*)dst->imageData)[dstIdx] = temp_alpha;
                  break;
                case 8:
                  ((uint64_t*)dst->imageData)[dstIdx] = temp_alpha;
                  break;
              }
            }
            else
            {
              //printf("X");
              switch(d)
              {
                case 1:
                  ((uint8_t*)dst->imageData)[dstIdx] = ((uint8_t*)src->imageData)[srcIdx];
                  break;
                case 2:
                  ((uint16_t*)dst->imageData)[dstIdx] = ((uint16_t*)src->imageData)[srcIdx];
                  break;
                case 4:
                  ((uint32_t*)dst->imageData)[dstIdx] = ((uint32_t*)src->imageData)[srcIdx];
                  break;
                case 8:
                  ((uint64_t*)dst->imageData)[dstIdx] = ((uint64_t*)src->imageData)[srcIdx];
                  break;
              }
            }
	  }
	}
      }
      break;
    }
    case CV_BGR2GRAY: //6
    case CV_RGB2GRAY: //7
    case CV_BGRA2GRAY: //10
    case CV_RGBA2GRAY: //11
    {
      for(int i = 0; i < h; i++)
      {
        //printf("\n%d:", i);
        for(int j = 0; j < w; j++)
        {
          //printf("%d ", j);
          int srcOffs = i * src->widthStep + j * srcNChan;
          int dstOffs = i * dst->widthStep + j;
          int srcIdx = srcOffs;
          int dstIdx = dstOffs;
          switch(d)
          {
            case 1:
              ((uint8_t*)dst->imageData)[dstIdx] =  wR * ((uint8_t*)src->imageData)[srcIdx + srcIChan[0]] +
                                                    wG * ((uint8_t*)src->imageData)[srcIdx + srcIChan[1]] +
                                                    wB * ((uint8_t*)src->imageData)[srcIdx + srcIChan[2]];
              break;
            case 2:
              ((uint16_t*)dst->imageData)[dstIdx] = wR * ((uint16_t*)src->imageData)[srcIdx + srcIChan[0]] +
                                                    wG * ((uint16_t*)src->imageData)[srcIdx + srcIChan[1]] +
                                                    wB * ((uint16_t*)src->imageData)[srcIdx + srcIChan[2]];
              break;
            case 4:
              ((uint32_t*)dst->imageData)[dstIdx] = wR * ((uint32_t*)src->imageData)[srcIdx + srcIChan[0]] +
                                                    wG * ((uint32_t*)src->imageData)[srcIdx + srcIChan[1]] +
                                                    wB * ((uint32_t*)src->imageData)[srcIdx + srcIChan[2]];
              break;
            case 8:
              ((uint64_t*)dst->imageData)[dstIdx] = wR * ((uint64_t*)src->imageData)[srcIdx + srcIChan[0]] +
                                                    wG * ((uint64_t*)src->imageData)[srcIdx + srcIChan[1]] +
                                                    wB * ((uint64_t*)src->imageData)[srcIdx + srcIChan[2]];
              break;
	  }
	}
      }
      break;
    }
    default:
    {
      fprintf(stdout, "%s:%s: Error: conversion option = %d not implemented.\n", __FILE__, __FUNCTION__, code);
      exit(1);
    }
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
    sprintf(ext, "%s", filename + strlen(filename) - len);
    for (int i = 0; i < len; i++)
    {
      ext[i] = toupper(ext[i]);
    }
    return ext;
  }
  return NULL;
}


IplImage* iplLoadImage(char* filename, int iscolor /*= CV_LOAD_IMAGE_UNCHANGED*/)
{
  IplImage* iplImage;

  char* ext = getFileExtensionUppercase(filename);
  if      ( strcmp(ext, ".PGM") == 0 || strcmp(ext, ".PPM") == 0 )
  {
    iplImage = iplLoadPgm(filename);
  }

  else if ( strcmp(ext, "TIFF") == 0 || strcmp(ext, ".TIF") == 0 )
  {
#ifdef __TIFF__
    iplImage = iplLoadTiff(filename);
#else
    fprintf(stderr, "%s:%s: Error: TIFF format unsupported. Please recompile using WITH_TIFF = 1.\n", __FILE__, __FUNCTION__);
    exit(1);
#endif
  }
  
  else
  {
    fprintf(stderr, "%s:%s: Error: extension %s unsupported. You may try to recompile using WITH_OPENCV = 1 or use iplLoadPgm instead.\n", __FILE__, __FUNCTION__, ext);
    exit(1);
  }

  if (iplImage)
  {
    int srcNChan = iplImage->nChannels;
    int dstNChan;
    int depth = iplImage->depth;
    CvSize size = cvGetSize(iplImage);
    int code;
    IplImage* dst;

    switch (iscolor)
    {
      case CV_LOAD_IMAGE_UNCHANGED:
      {
        return iplImage;
        break;
      }
      case CV_LOAD_IMAGE_GRAYSCALE:
      {
        dstNChan = 1;
        switch (srcNChan)
	{
          case 1:
            return iplImage;
            break;
          case 3:
            code = CV_RGB2GRAY;
            break;
          case 4:
            code = CV_RGBA2GRAY;
            break;
          default:
            fprintf(stderr, "%s:%s: Error: loaded image has number of channels = %d unsupported by iplCvtColor. You may try to recompile using WITH_OPENCV = 1.\n", __FILE__, __FUNCTION__, srcNChan);
            exit(1);
	  }
        break;
      }
      case CV_LOAD_IMAGE_COLOR:
      {
        dstNChan = 3;
        switch (srcNChan)
	{
          case 1:
            code = CV_GRAY2BGR;
            break;
          case 3:
            code = CV_RGB2BGR;
            break;
          case 4:
            code = CV_RGBA2BGR;
            break;
          default:
            fprintf(stderr, "%s:%s: Error: loaded image has number of channels = %d unsupported by iplCvtColor. You may try to recompile using WITH_OPENCV = 1.\n", __FILE__, __FUNCTION__, srcNChan);
            exit(1);
	  }
        break;
      }
      default:
      {
        fprintf(stdout, "%s:%s: Error: load option = %d not implemented.\n", __FILE__, __FUNCTION__, iscolor);
        exit(1);
      }
    }

    dst = iplCreateImage(size, depth, dstNChan);

    iplCvtColor(iplImage, dst, code);
    iplReleaseImage(&iplImage);
    return dst;
  }
  else
  {
    fprintf(stderr, "%s:%s: Error loading image from file %s.\n", __FILE__, __FUNCTION__, filename);
    exit(1);
  }
}

int iplSaveImage(char* filename, IplImage* image, int* params /*=0*/)
{
  IplImage* iplImage;
  int result;

  char* ext = getFileExtensionUppercase(filename);

  if      ( strcmp(ext, ".PGM") == 0 || strcmp(ext, ".PPM") == 0 )
  {
    result = iplSavePgm(filename, image);
  }

  else if ( strcmp(ext, "TIFF") == 0 || strcmp(ext, ".TIF") == 0 )
  {
#ifdef __TIFF__
    result = iplSaveTiff(filename, image);
#else
    fprintf(stderr, "%s:%s: Error: TIFF format unsupported. Please recompile using WITH_TIFF = 1.\n", __FILE__, __FUNCTION__);
    exit(1);
#endif
  }

  else
  {
    fprintf(stderr, "%s:%s: Error: extension %s unsupported. You may try to recompile using WITH_OPENCV = 1 or use iplSavePgm instead.\n", __FILE__, __FUNCTION__, ext);
    exit(1);
  }

  return result;
}

