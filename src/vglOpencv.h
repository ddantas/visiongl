/*********************************************************************
***                                                                ***
***  Header file vglOpencv                                         ***
***                                                                ***
*********************************************************************/

#ifndef __VGLOPENCV_H__
#define __VGLOPENCV_H__

#ifndef __OPENCV__

/*********************************************************************
***  IplImage                                                      ***
*********************************************************************/

enum
{
    CV_BGR2BGRA    =0,
    CV_RGB2RGBA    =CV_BGR2BGRA,

    CV_BGRA2BGR    =1,
    CV_RGBA2RGB    =CV_BGRA2BGR,

    CV_BGR2RGBA    =2,
    CV_RGB2BGRA    =CV_BGR2RGBA,

    CV_RGBA2BGR    =3,
    CV_BGRA2RGB    =CV_RGBA2BGR,

    CV_BGR2GRAY    =6,
    CV_RGB2GRAY    =7,
};

enum
{
/* 8bit, color or not */
    CV_LOAD_IMAGE_UNCHANGED  =-1,
/* 8bit, gray */
    CV_LOAD_IMAGE_GRAYSCALE  =0,
/* ?, color */
    CV_LOAD_IMAGE_COLOR      =1,
/* any depth, ? */
    CV_LOAD_IMAGE_ANYDEPTH   =2,
/* ?, any color */
    CV_LOAD_IMAGE_ANYCOLOR   =4
};

/*
 * The following definition
 * is an extract from IPL headers.
 * Copyright (c) 1995 Intel Corporation.
 */
#define IPL_DEPTH_SIGN 0x80000000

#define IPL_DEPTH_1U     1
#define IPL_DEPTH_8U     8
#define IPL_DEPTH_16U   16
#define IPL_DEPTH_32F   32

#define IPL_DEPTH_8S  (IPL_DEPTH_SIGN| 8)
#define IPL_DEPTH_16S (IPL_DEPTH_SIGN|16)
#define IPL_DEPTH_32S (IPL_DEPTH_SIGN|32)

typedef struct _IplImage
{
    int  nSize;             /* sizeof(IplImage) */
    int  ID;                /* version (=0)*/
    int  nChannels;         /* Most of OpenCV functions support 1,2,3 or 4 channels */
    int  alphaChannel;      /* Ignored by OpenCV */
    int  depth;             /* Pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                               IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported.  */
    char colorModel[4];     /* Ignored by OpenCV */
    char channelSeq[4];     /* ditto */
    int  dataOrder;         /* 0 - interleaved color channels, 1 - separate color channels.
                               cvCreateImage can only create interleaved images */
    int  origin;            /* 0 - top-left origin,
                               1 - bottom-left origin (Windows bitmaps style).  */
    int  align;             /* Alignment of image rows (4 or 8).
                               OpenCV ignores it and uses widthStep instead.    */
    int  width;             /* Image width in pixels.                           */
    int  height;            /* Image height in pixels.                          */
    struct _IplROI *roi;    /* Image ROI. If NULL, the whole image is selected. */
    struct _IplImage *maskROI;      /* Must be NULL. */
    void  *imageId;                 /* "           " */
    struct _IplTileInfo *tileInfo;  /* "           " */
    int  imageSize;         /* Image data size in bytes
                               (==image->height*image->widthStep
                               in case of interleaved data)*/
    char *imageData;        /* Pointer to aligned image data.         */
    int  widthStep;         /* Size of aligned image row in bytes.    */
    int  BorderMode[4];     /* Ignored by OpenCV.                     */
    int  BorderConst[4];    /* Ditto.                                 */
    char *imageDataOrigin;  /* Pointer to very origin of image data
                               (not necessarily aligned) -
                               needed for correct deallocation */
}
IplImage;


/*********************************************************************
***  CvSize                                                        ***
*********************************************************************/

typedef struct CvSize
{
    int width;
    int height;
}
CvSize;

inline CvSize cvSize(int width, int height)
{
  CvSize s;
  s.width = width;
  s.height = height;
  return s;
}

inline CvSize cvGetSize(IplImage* ipl)
{
  return cvSize(ipl->width, ipl->height);
}

/*********************************************************************
***  Function prototypes                                           ***
*********************************************************************/

void cvReleaseImage(IplImage** p_ipl);
IplImage* cvCreateImage(CvSize size, int depth, int channels);
IplImage* cvCopy(IplImage* src, IplImage* dst);
void cvCvtColor(IplImage* src, IplImage* dst, int code);
IplImage* cvLoadImage(char* filename, int iscolor = CV_LOAD_IMAGE_UNCHANGED);
int cvSaveImage(char* filename, IplImage* image, int* params = 0);


CvSize cvSize(int width, int height);
CvSize cvGetSize(IplImage* ipl);


#endif

#endif
