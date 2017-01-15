/*********************************************************************
***                                                                ***
***  Header file iplImage                                          ***
***                                                                ***
*********************************************************************/

#ifndef __IPLIMAGE_H__
#define __IPLIMAGE_H__

#ifdef __OPENCV__
  #include <opencv2/core/types_c.h>
#else
  #include <vglOpencv.h>
#endif


/*********************************************************************
***  Function prototypes                                           ***
*********************************************************************/

int iplFindBitsPerSample(int depth);
int iplFindWidthStep(int depth, int width, int channels = 1);

void iplReleaseImage(IplImage** p_ipl);
IplImage* iplCreateImage(CvSize size, int depth, int channels);
IplImage* iplCopy(IplImage* src, IplImage* dst);
void iplCvtColor(IplImage* src, IplImage* dst, int code);

IplImage* iplLoadPgm(char* filename);
IplImage* iplLoadImage(char* filename, int iscolor = CV_LOAD_IMAGE_UNCHANGED);

int iplGenericSavePgm(char* filename, char* buf, int w, int h, int widthStep, int c, int b);
int iplSavePgm(char* filename, IplImage* ipl);
int iplSaveImage(char* filename, IplImage* image, int* params = 0);



#endif
