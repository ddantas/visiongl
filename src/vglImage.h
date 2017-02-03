/*********************************************************************
***                                                                ***
***  Header file vglImage                                          ***
***                                                                ***
*********************************************************************/

#ifndef __VGLIMAGE_H__
#define __VGLIMAGE_H__


#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT 0x8CD8
#define GL_FRAMEBUFFER_STATUS_ERROR_EXT 0x8CDE


#define __is_pod(type) 1
#define __is_empty(type) 1
#define __has_trivial_destructor(type) 1
//extern void* __builtin_memchr(const void*__s, int __a, unsigned int __n);
#include <stdarg.h>

#include <cstdio>

//opencv IplImage
#ifdef __OPENCV__
  #undef __SSE2__
  #include <opencv2/imgproc/types_c.h>
  #include <opencv2/highgui/highgui_c.h>
#else
  #include <vglOpencv.h>
#endif

//GL
#include <GL/glew.h>
#include <GL/freeglut.h>

//CL
#ifdef __OPENCL__
#include <CL/cl.h>
#endif

//assert
#include <assert.h>

// VGL HEADER - Vision with OpenGL


//VisionGL
#include <vglConst.h>
#include <vglShape.h>

////////// VglImage

class VglImage{
 public:
  IplImage* ipl;
  void*     ndarray;
  int       ndim;                // 2 if conventional image, 3 if three-dimensional etc
  int       shape[2*VGL_MAX_DIM];  // shape[0] = width, shape[1] = height, shape[2] = number of frames etc
  VglShape* vglShape;
  int       depth;
  int       nChannels;
  int       has_mipmap;
  GLuint    fbo;
  GLuint    tex;
#ifdef __CUDA__
  void*     cudaPtr;
  GLuint    cudaPbo;
#endif
#ifdef __OPENCL__
  cl_mem    oclPtr;
  int       clForceAsBuf;
#endif
  int       inContext;
  char*     filename;

  size_t getBitsPerSample()
  {
    return this->depth & 255;
  }

  /** widthStep in bytes

   */
  int getWidthStep()
  {
    int widthStep;
    /*if (this->ipl) // TODO: fix this
    {
      widthStep = this->ipl->widthStep;
    }
    else*/
    {
      int bps = this->getBitsPerSample();
      if (bps == 1)
      {
        widthStep = (this->getWidthIn() - 1) / 8 + 1;
      }
      else if (bps < 8)
      {
        fprintf(stderr, "%s:%s: Error: bits per pixel = %d < 8 and != 1. Image depth may be wrong.\n", __FILE__, __FUNCTION__, bps);
        exit(1);
      }
      else
      {
        widthStep = (bps / 8) * this->getNChannels() * this->getWidthIn();
      }
    }
    return widthStep;
  }

  /** widthStep in words

   */
  int getWidthStepWords()
  {
    return (this->getWidthStep() - 1) / VGL_PACK_SIZE_BYTES + 1;
  }
 
  /** Total number of rows

      Get total number of rows. Notice that, in images with more than 2D, may be 
      different of image height
  */
  size_t getTotalRows()
  {
    return this->vglShape->getHeightIn() * this->vglShape->getNFrames();
  }

  /** Get row size in bytes

      This is the same as widthStep and can be eliminated.
  */
  size_t getRowSizeInBytes()
  {
      int bps = this->getBitsPerSample();
      if (bps == 1)
      {
        return this->getWidthStep();
      }
      else if (bps < 8)
      {
        fprintf(stderr, "%s:%s: Error: bits per pixel = %d < 8 and != 1. Image depth may be wrong.\n", __FILE__, __FUNCTION__, bps);
        exit(1);
      }
      return this->getWidthStep();
  }

  size_t getTotalSizeInBytes()
  {
    return this->getTotalRows() * this->getRowSizeInBytes();
  }

  char* getImageData()
  {
    if (this->ndarray)
    {
      return (char*) this->ndarray;
    }
    else if (this->ipl)
    {
      return (char*) this->ipl->imageData;
    }
    else
    {
      fprintf(stderr, "%s: %s: Error: no pointer to raster image data available.\n", __FILE__, __FUNCTION__);
      return NULL;
    }     
  }

  int getNChannels()
  {
    return this->vglShape->shape[VGL_SHAPE_NCHANNELS];
  }

  int getWidth()
  {
    return this->vglShape->getWidth();
  }

  int getHeight()
  {
    return this->vglShape->getHeight();
  }

  int getLength()
  {
    return this->vglShape->getLength();
  }

  int getWidthIn()
  {
    return this->vglShape->getWidthIn();
  }

  int getHeightIn()
  {
    return this->vglShape->getHeightIn();
  }

  int getNFrames()
  {
    return this->vglShape->getNFrames();
  }


};

////////// VglNamedWindow

class VglNamedWindow  {
public:
  VglImage* image;
  char* name;
};

////////// vglNamedWindowList


class VglNamedWindowList  {

public:
  VglNamedWindowList(void);
  ~VglNamedWindowList(void);
  //void Init();
  int MaxWindows();
  int CreateNamedSubwindow(char* winname);
  int CreateSubwindow(void);
  int ShowImage(char* name, VglImage* image);
  void Reset(void);
  int Cycle(void);
  void Refresh(int win_index, int split = VGL_DEFAULT_WINDOW_SPLIT);
  void RefreshAll(int split = VGL_DEFAULT_WINDOW_SPLIT);
  int main_window_id;
  int NamedWindow(char* winname);

private:
  VglNamedWindow WindowList[VGL_MAX_WINDOWS];
  int FreePosition[VGL_MAX_WINDOWS];
  int WindowIdByName(char* name);
  int current_window;
};

////////// vglImage functions

//void vglShowImage(char* winname, VglImage* image)
//int vglNamedWindow(char* winname);


int vglInit();
int vglInit(int w, int h);
void vglUpload(VglImage* image, int swapRGB = 0);
VglImage* vglCopyCreateImage(VglImage* img_in);
VglImage* vglCreateImage(VglImage* img_in);
VglImage* vglCopyCreateImage(IplImage* img_in, int ndim = 2, int has_mipmap = 0);
VglImage* vglCreateImage(IplImage* img_in, int ndim = 2, int has_mipmap = 0);
VglImage* vglCreateImage(int* shape, int depth, int ndim = 2, int has_mipmap = 0);
VglImage* vglCreateImage(VglShape* vglShape, int depth, int has_mipmap = 0);
VglImage* vglCreateImage(CvSize size, int depth = IPL_DEPTH_8U, int nChannels = 3, int ndim = 2, int has_mipmap = 0);
VglImage* vglCreate3dImage(CvSize size, int depth, int nChannels, int nlength, int has_mipmap = 0);
VglImage* vglCreateNdImage(int ndim, int* shape, int depth, int has_mipmap = 0);
void vglSaveImage(char* filename, VglImage* image);
void vglSave3dImage(char* filename, VglImage* image, int lStart, int lEnd = -1);
void vglSaveNdImage(char* filename, VglImage* image, int lStart, int lEnd = -1);
VglImage* vglCloneImage(IplImage* img_in, int ndim = 2, int has_mipmap = 0);
void vglReleaseImage(VglImage** p_image);
void vglReplaceIpl(VglImage* image, IplImage* new_ipl);
void vglDownload(VglImage* image);
void vglDownloadPPM(VglImage* image);
void vglDownloadFBO(VglImage* image);
void vglDownloadFaster(VglImage* image/*, VglImage* buf*/);
VglImage* vglLoadImage(char* filename, int iscolor = -1, int has_mipmap = 0); // -1 = CV_LOAD_IMAGE_UNCHANGED
VglImage* vglLoad3dImage(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
VglImage* vglLoadNdImage(char* filename, int lStart, int lEnd, int* shape, int ndim, bool has_mipmap = 0);
int vglReshape(VglImage* img, VglShape* newShape);
void vglPrintImageData(VglImage* image, char* msg = NULL, char* format = (char*) "%c");
void iplPrintImageData(IplImage* image, char* msg = NULL, char* format = (char*) "%c");
void vglPrintImageInfo(VglImage* image, char* msg = NULL);
void iplPrintImageInfo(IplImage* ipl, char* msg = NULL);
void vglCopyImageTex(VglImage* src, VglImage* dst);
void vglAxis(void);
void vglCopyImageTexVFS(VglImage* src, VglImage* dst);
void vglCopyImageTexFS(VglImage* src, VglImage* dst);
void vglGreen(VglImage* src, VglImage* dst);
void vglVerticalFlip2(VglImage* src, VglImage* dst);
void vglHorizontalFlip2(VglImage* src, VglImage* dst);
void vglClear(VglImage* image, float r, float g, float b, float a = 0.0);
void vglNdarray3To4Channels(VglImage* img);
void vglNdarray4To3Channels(VglImage* img);
void vglIpl3To4Channels(VglImage* img);
void vglIpl4To3Channels(VglImage* img);
void vglImage3To4Channels(VglImage* img);
void vglImage4To3Channels(VglImage* img);
int SavePPM(char* filename, int w, int h, void* savebuf);
int vglSavePPM(char* filename, VglImage* img);
int vglSavePgm(char* filename, VglImage* img);
VglImage* vglLoadPgm(char* filename);
int vglHasDisplay();


void vglDistTransformCross3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglDistTransformSq3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglGetLevelDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglThinBernard(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglThinChin(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglBaricenterVga(VglImage* src, double* x_avg = NULL, double* y_avg = NULL, double* pix_count = NULL);
void vglCloseSq3(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglOpenSq3 (VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglErodeSq3Sep(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglErodeSq5Sep(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglCErodeCross3(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buf, int times);
void vglGray2(VglImage*  src, VglImage*  dst, VglImage*  dst1);

#ifdef __OPENCL__
void vglClForceAsBuf(VglImage*  img);
#endif

void vglInOut_model(VglImage*  dst, VglImage*  dst1);

////////// Macros

#define CHECK_FRAMEBUFFER_STATUS() \
{ \
 /*printf("CHECK_FRAMENBUFFER_STATUS\n");*/ \
 GLenum status; \
 status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
 switch(status) { \
 case GL_FRAMEBUFFER_COMPLETE_EXT: \
   break; \
 case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
   printf("framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n"); \
    /* you gotta choose different formats */ \
   assert(0); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
   printf("framebuffer INCOMPLETE_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
   printf("framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
   printf("framebuffer FRAMEBUFFER_DIMENSIONS\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT: \
   printf("framebuffer INCOMPLETE_DUPLICATE_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
   printf("framebuffer INCOMPLETE_FORMATS\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
   printf("framebuffer INCOMPLETE_DRAW_BUFFER\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
   printf("framebuffer INCOMPLETE_READ_BUFFER\n"); \
   break; \
 case GL_FRAMEBUFFER_BINDING_EXT: \
   printf("framebuffer BINDING_EXT\n"); \
   break; \
 case GL_FRAMEBUFFER_STATUS_ERROR_EXT: \
   printf("framebuffer STATUS_ERROR\n"); \
   break; \
 default: \
   printf("programming error; will fail on all hardware\n"); \
   assert(0); \
 } \
} \

#define ERRCHECK() \
{ \
  static GLenum errCode; \
  const GLubyte *errString; \
  if ((errCode = glGetError()) != GL_NO_ERROR) { \
    /*errString = gluErrorString(errCode);*/     \
    errString = glGetString(errCode); \
    printf ("OpenGL Error %x: %s at %s:%d\n", errCode, errString,  __FILE__,__LINE__); \
	exit(1); \
  } \
} \
 


#endif
