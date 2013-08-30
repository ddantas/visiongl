
#ifndef __VISIONGL_H__
#define __VISIONGL_H__

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
#undef __SSE2__
#include <opencv2/core/types_c.h>

//GL
#include <GL/freeglut_std.h>
#include <GL/freeglut_ext.h>


#define VGL_WIN_X0   -1.0
#define VGL_WIN_X1    1.0
#define VGL_WIN_DX    (VGL_WIN_X1 - VGL_WIN_X0)
#define VGL_WIN_Y0   -1.0
#define VGL_WIN_Y1    1.0
#define VGL_WIN_DY    (VGL_WIN_Y1 - VGL_WIN_Y0)


// VGL HEADER - Vision with OpenGL


#define VGL_MIN_WINDOW_SPLIT 1
#define VGL_DEFAULT_WINDOW_SPLIT 2
#define VGL_MAX_WINDOW_SPLIT 4
#define VGL_MAX_WINDOWS      (VGL_MAX_WINDOW_SPLIT * VGL_MAX_WINDOW_SPLIT)

////////// VglImage

class VglImage{
 public:
  IplImage* ipl;
  int       width;
  int       height;
  int       depth;
  int       dim3;
  int       nChannels;
  int       has_mipmap;
  GLuint    fbo;
  GLuint    tex;
  void*     cudaPtr;
  GLuint    cudaPbo;
  int       inContext;
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
VglImage* vglCopyCreateImage(IplImage* img_in, int dim3 = 1, int has_mipmap = 0);
VglImage* vglCreateImage(IplImage* img_in, int dim3 = 1, int has_mipmap = 0);
VglImage* vglCreateImage(CvSize size, int depth = IPL_DEPTH_8U, int nChannels = 3, int dim3 = 1, int has_mipmap = 0);
VglImage* vglCloneImage(IplImage* img_in, int dim3 = 1, int has_mipmap = 0);
void vglReleaseImage(VglImage** p_image);
void vglReplaceIpl(VglImage* image, IplImage* new_ipl);
void vglDownload(VglImage* image);
void vglDownloadPPM(VglImage* image);
void vglDownloadFBO(VglImage* image);
void vglDownloadFaster(VglImage* image/*, VglImage* buf*/);
VglImage* vglLoadImage(char* filename, int iscolor = 1, int has_mipmap = 0);
void vglPrintImageInfo(VglImage* image);
void iplPrintImageInfo(IplImage* ipl);
void vglCopyImageTex(VglImage* src, VglImage* dst);
void vglAxis(void);
void vglCopyImageTexVFS(VglImage* src, VglImage* dst);
void vglCopyImageTexFS(VglImage* src, VglImage* dst);
void vglGreen(VglImage* src, VglImage* dst);
void vglVerticalFlip2(VglImage* src, VglImage* dst);
void vglHorizontalFlip2(VglImage* src, VglImage* dst);
void vglClear(VglImage* image, float r, float g, float b, float a = 0.0);
int SavePPM(char* filename, int w, int h, void* savebuf);
int vglSavePPM(VglImage* img, char* filename);
int SavePGM(char* filename, int w, int h, void* savebuf);
int vglSavePGM(VglImage* img, char* filename);
IplImage* LoadPGM(char* filename);
VglImage* vglLoadPGM(char* filename);


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

void vglInOut_model(VglImage*  dst, VglImage*  dst1);

////////// Macros

#define CHECK_FRAMEBUFFER_STATUS() \
{\
 GLenum status; \
 status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
 switch(status) { \
 case GL_FRAMEBUFFER_COMPLETE_EXT: \
   break; \
 case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
   printf("framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");\
    /* you gotta choose different formats */ \
   assert(0); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
   printf("framebuffer INCOMPLETE_ATTACHMENT\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
   printf("framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
   printf("framebuffer FRAMEBUFFER_DIMENSIONS\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT: \
   printf("framebuffer INCOMPLETE_DUPLICATE_ATTACHMENT\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
   printf("framebuffer INCOMPLETE_FORMATS\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
   printf("framebuffer INCOMPLETE_DRAW_BUFFER\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
   printf("framebuffer INCOMPLETE_READ_BUFFER\n");\
   break; \
 case GL_FRAMEBUFFER_BINDING_EXT: \
   printf("framebuffer BINDING_EXT\n");\
   break; \
 case GL_FRAMEBUFFER_STATUS_ERROR_EXT: \
   printf("framebuffer STATUS_ERROR\n");\
   break; \
 default: \
   /* programming error; will fail on all hardware */ \
   assert(0); \
 }\
}

#define ERRCHECK() \
{\
  static GLenum errCode;\
  const GLubyte *errString;\
  if ((errCode = glGetError()) != GL_NO_ERROR) {\
    errString = gluErrorString(errCode);\
    printf ("OpenGL Error: %s at %s:%d\n", errString,  __FILE__,__LINE__);\
    exit(1);\
  }\
}\
 

#endif
/*********************************************************************
***                                                                ***
***  Header file vglCudaImage.h                                    ***
***                                                                ***
*********************************************************************/


int vglCudaToGl(VglImage* img);
int vglGlToCuda(VglImage* img);
int vglCudaAlloc(VglImage* img);
int vglCudaFree(VglImage* img);

void vglCudaInvertOnPlace(VglImage* img);
int vglCudaAllocPbo(VglImage* img);
int vglCudaFreePbo(VglImage* img);
int vglCudaMapPbo(VglImage* img);
int vglCudaUnmapPbo(VglImage* img);
void vglCudaCopy(VglImage* img, VglImage* dst);
void vglCudaCopy2(VglImage* img, VglImage* dst);
void vglCudaInvert(VglImage* img, VglImage* dst);


/*********************************************************************
***                                                                ***
***  Header file vglContext.h                                      ***
***                                                                ***
***  ddantas 4/2/2009                                              ***
***                                                                ***
*********************************************************************/


#define VGL_BLANK_CONTEXT 0
#define VGL_RAM_CONTEXT 1
#define VGL_GL_CONTEXT 2
#define VGL_CUDA_CONTEXT 4

#define vglIsContextValid(x) ( (x>=1) and (x<=7) )
#define vglIsContextUnique(x) ( (x>=0) and (x<=4) and (x!=3) )
#define vglIsInContext(img, x) ( (img)->inContext & (x) or ((img)->inContext==0 and x==0))

int vglAddContext(VglImage* img, int context);
int vglSetContext(VglImage* img, int context);
int vglCheckContext(VglImage* img, int context);
int vglCheckContextForOutput(VglImage* img, int context);
void vglPrintContext(int context, char* msg = "");
void vglPrintContext(VglImage* img, char* msg = "");
/*********************************************************************
***                                                                ***
***  Header file vglSimpleBGModel.h                                ***
***                                                                ***
*********************************************************************/


#define VGL_SIMPLEBG_STD_THRESHOLD      2.5   // lambda=2.5 is 99% 
#define VGL_SIMPLEBG_WINDOW_SIZE        200   // learning rate; alpha = 1/WINDOW_SIZE 
#define VGL_SIMPLEBG_TRAINING_IMAGES    50    // number of images used to initialize bg model 

class VglSimpleBGModel
{
private:
  int width;
  int height;
  int nChannels;
  int depth;

  // UpdateSimpleBGModel
  VglImage* buf;
  // SimpleBGModelUpdateXGY
  VglImage* sobel;
  VglImage* color;
  VglImage* grayx;
  VglImage* grayy;


public:
  float std_thresh;
  int window_size;
  int num_training_images; 
  int curr_training_image;
  VglImage* foreground;
  VglImage* foregroundClose;
  VglImage* background32;
  VglImage* variance32;

  VglSimpleBGModel(VglImage* img_in, float std_thresh, int window_size, int num_training_images);
  ~VglSimpleBGModel();
  void TrainSimpleBGModel(VglImage* img_in);
  void DetectFGSimpleBGModel(VglImage* img_in);
  void DetectFGSimpleBGModel2(VglImage* img_in);
  void UpdatePartialSimpleBGModel(VglImage* img_in);
  void UpdateSimpleBGModel(VglImage* img_in);
  void UpdateSimpleBGModelXGY(VglImage* img_in);
  void UpdateSimpleBGModelRGL(VglImage* img_in);
};




/*********************************************************************
***                                                                 ***
***  Source code generated by glsl2cpp.pl                           ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/
/** Detects foreground pixels.

  */
void vglDetectFGSimpleBGModel(VglImage*  img_in, VglImage*  average, VglImage*  variance, VglImage*  foreground, float  std_thresh);

/** Updates average and variance of background model.

  */
void vglTrainSimpleBGModel(VglImage*  img_in, VglImage*  average, VglImage*  variance, float  weight);

/** Updates average and variance of background model only in pixels that are classified as background.

  */
void vglUpdatePartialSimpleBGModel(VglImage*  img_in, VglImage*  foregorundClose, VglImage*  average, VglImage*  variance, float  weight);


/*********************************************************************
***                                                                 ***
***  Source code generated by glsl2cpp.pl                           ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/
/** vglForcesNewton

    Iterates n masses using newtons formula.

    In this step, the forces are calculated.

  */
void vglForcesNewton(VglImage*  src, VglImage*  dst);

/** vglPosNewton

    Iterates n masses using newtons formula.

    In this step, the positions and speed are are calculated 
    from the force.

  */
void vglPosNewton(VglImage*  src, VglImage*  dst);


/*********************************************************************
***                                                                 ***
***  Source code generated by glsl2cpp.pl                           ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/
/** Calculate absolute difference between img_ref and img_2. Disparities considered are in the closed interval [4*disparity, 4*disparity+3].

    The four differences are stored in the RGBA image dst.

  */
void vglAbsDiffDisparity(VglImage*  img_ref, VglImage*  img_2, VglImage*  dst, float  disparity);

/** Calculates average absolute difference between img_ref and img_2 at levels of detail in [0, max_lod]. Disparities considered are in the closed interval [4*disparity, 4*disparity+3].

    The four differences are stored in the RGBA image dst.

  */
void vglAbsDiffDisparityMipmap(VglImage*  img_ref, VglImage*  img_2, VglImage*  dst, float  disparity, float  max_lod);

/** Find best disparity. The first input image, img_dif, contains absolute differences between a pair of images at disparities [4*disparity, 4*disparity+3]. 

    The second input image contains the smallest differences found in channel R, and corresponding disparity value in channel A, Is also an output image, and is updated whenever a smaller difference is found.

  */
void vglFindDisparity(VglImage*  img_dif, VglImage*  img_disp, float  disparity);

/** Do the same as vglFindDisparity, but the smallest difference is stored in img_best, and corresponding disparity in img_disp. Both are input and output images.

  */
void vglFindDisparityDiff(VglImage*  img_sum, VglImage*  img_disp, VglImage*  img_best, float  disparity);

/** Calculate absolute difference between green channel of img_ref and img_2. Disparities considered are in the closed interval [4*disparity, 4*disparity+3].

    The four differences are stored in the RGBA image dst.

  */
void vglGreenDiffDisparity(VglImage*  img_ref, VglImage*  img_2, VglImage*  dst, float  disparity);

/** Apply homography in img_src and stores result in img_dst.

    Important: for matrices the cmponents are written in 
    column major order: 

    \c mat2 \c m = \c mat2 (1, 2, 3, 4)
\f$
    \Leftrightarrow
\f$
    \c m = 
\f$
    \left( 
      \begin{array}{cc}
          1  &  3        \\

          2  &  4
      \end{array}
    \right) 

\f$

    In C we build the matrix in line major order, then we must transpose
    tbe matrix before using it in OpenGL context.

  */
void vglHomography(VglImage*  img_src, VglImage*  img_dst, float*  f_homo);

/** Convert depth map to affine reconstruction

    This algorithm ignores the infinite homography.

    img_map: input depth map

    img_3d: output reconstruction

    f: focal length in pixels

    b: baseline in cm

    D: fixation point or maximum depth

    is_float: if true, output image will store z in cm. If false
              output image will store z as 255 * (depth / D). 
              if depth == D then z = 0.

    disp_k: If set, single disparity will be used.

    h: height of camera in cm
 */
void vglMapTo3D(VglImage*  img_map, VglImage*  img_3d, float  f, float  b, float  D, float  disp_k= 0.0, float  h= 10.0);

/** Mean of pixel values of levels of detail in [0, max_lod]. Result is stored in img_out.


  */
void vglMeanMipmap(VglImage*  img_dif, VglImage*  img_out, float  max_lod);

/** Mean filter with a 3x3 square mask.

  */
void vglMeanSq3(VglImage*  img_dif, VglImage*  img_out);

/** Undistort, correct projection and rectify img_src and stores result in img_dst, for use with stereo algorithm

    The input float array f_dist contains the coefficient of radial distortion, and f_proj contains the intrinsinc parameters of the camera: center of projection (x and y); focal length in pixels (x and y). The focal lengths are the same when the pixels are square.

    The input float array f_homo contains the homography that rectifies the image.

    Important: for matrices the cmponents are written in 
    column major order: 

    \c mat2 \c m = \c mat2 (1, 2, 3, 4)
\f$
    \Leftrightarrow
\f$
    \c m = 
\f$
    \left( 
      \begin{array}{cc}
          1  &  3        \\

          2  &  4
      \end{array}
    \right) 

\f$


    In C we build the matrix in line major order, then we must transpose
    tbe matrix before using it in OpenGL context.

  */
void vglRectify(VglImage*  img_src, VglImage*  img_dst, float*  f_dist, float*  f_proj, float*  f_homo);

/** VglSumDiff

    Sum of differences

  */
void vglSumDiff(VglImage*  img_dif, VglImage*  img_out);

/** VglSumDiffMipmap

    Sum of differences

  */
void vglSumDiffMipmap(VglImage*  img_dif, VglImage*  img_out, float  max_lod);

/** Correct camera lens distortion of img_src and stores the result in img_dst.

    The input float array f_dist contains the coefficient of radial distortion, and f_proj contains the intrinsinc parameters of the camera: center of projection (x and y); focal length in pixels (x and y). The focal lengths are the same when the pixels are square.

    Reference:

    http://www.cognotics.com/opencv/docs/1.0/ref/opencvref_cv.htm#cv_3d

  */
void vglUndistort(VglImage*  img_src, VglImage*  img_dst, float*  f_dist, float*  f_proj);


/*********************************************************************
***                                                                 ***
***  Source code generated by glsl2cpp.pl                           ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/

void shader_15_1(VglImage*  src, VglImage*  dst);

/** Convert grayscale image to RGB

  */
void vgl1to3Channels(VglImage*  src, VglImage*  dst);

/** Absolute difference between two images.

  */
void vglAbsDiff(VglImage*  src0, VglImage*  src1, VglImage*  dst);

/** Logical AND between two images

  */
void vglAnd(VglImage*  src0, VglImage*  src1, VglImage*  dst);

/** Initialize image to be used in baricenter calculation. The initialization is done by storing the values (1, x, y) in each output pixel so that the summation over th whole image gives the three moments of the image.

    R =                    f(x, y) 

    G =                x * f(x, y) 

    B =                y * f(x, y) 

  */
void vglBaricenterInit(VglImage*  src, VglImage*  dst);

/** vglBlurSq3

    Blur image by 3x3 square structuring element.

  */
void vglBlurSq3(VglImage*  src, VglImage*  dst);

/** Clear image with given color.

  */
void vglClear2(VglImage*  src_dst, float  r, float  g, float  b, float  a= 0.0);

/** Changes contrast of image by given factor.

  */
void vglContrast(VglImage*  src, VglImage*  dst, float  factor);

/** Shows coordinates of pixels as colors. Red is horizontal and green is vertical. Coordinates and colors are defined by OpenGL, that is, between 0 and 1.

  */
void vglCoordToColor(VglImage*  dst);

/** Direct copy from src to dst.

  */
void vglCopy(VglImage*  src, VglImage*  dst);

/** Crossing number is defined as the number of ocurrences of the pattern 01
in the neihborhood of a pixel.

    Neighborhood of pixel P is indexed as follows:

\f$
\begin{array}{ccc}

    P3   &  P2   &  P1      \\ 
    P4   &  P    &  P0/8    \\
    P5   &  P6   &  P7

\end{array}
\f$


    References:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    T. M. Bernard and A. Manzanera, Improved low complexity fully parallel
        thinning algorithms, 1999

  */
void vglCrossingNumber(VglImage*  src, VglImage*  dst);

/** Deletes corner from skeleton.

    Receive as input the image with the skeleton to be thinned. Receives 
    also the step. must be called once with step 1 and once with step 2.

    Neighborhood pixels is indexed as follows:

\f$
\begin{array}{ccc}

    P3  &  P2  &  P1      \\
    P4  &  P8  &  P0      \\
    P5  &  P6  &  P7

\end{array}
\f$


    Pixels deleted are the ones that mach the pattern and its rotations 
    by 90deg.

\f$
\begin{array}{ccc}

    0  &  0  &  x      \\
    0  &  1  &  1      \\
    x  &  1  &  0

\end{array}
\f$

    References:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    T. M. Bernard and A. Manzanera, Improved low complexity fully parallel
        thinning algorithms, 1999

  */
void vglDeleteSkeletonCorners(VglImage*  src, VglImage*  dst, int  step);

/** Deletes warts from skeleton. Receive as input the image with the skeleton to be thinned. Neighborhood pixels are indexed as follows:

\f$
\begin{array}{ccc}

    P3  &  P2  &  P1      \\
    P4  &  P   &  P0/8    \\
    P5  &  P6  &  P7

\end{array}
\f$

    Pixels deleted are the ones that mach the pattern and its rotations 
    by 45deg.

\f$
\begin{array}{ccc}

    1  &  0  &  0    \\
    1  &  1  &  0    \\
    1  &  0  &  0

\end{array}
\f$

    That is the same as delete the pixels with crossing number = 1 and 
neighbor number = 3

    References:

    Ke Liu et al., Identification of fork points on the skeletons
        of handwritten chinese characters

  */
void vglDeleteSkeletonWarts(VglImage*  src, VglImage*  dst);

/** Deletes warts from skeleton. Receive as input the image with the skeleton to be thinned. Neighborhood pixels are indexed as follows:

    P3 P2 P1

    P4 P  P0/8

    P5 P6 P7

    Pixels deleted are the ones that mach the pattern and its rotations 
    by 45deg.

    1 0 0
    1 1 0
    1 0 0

    1 1 0
    1 1 0
    1 0 0

    1 1 0
    1 1 0
    1 1 0

    1 1 1
    1 1 0
    1 1 0

    1 1 1
    1 1 0
    1 1 1


    That is the same as delete the pixels with crossing number = 1 and 
neighbor number >=3

    References:

    Ke Liu et al., Identification of fork points on the skeletons
        of handwritten chinese characters

  */
void vglDeleteSkeletonWarts2(VglImage*  src, VglImage*  dst);

/** Image src0 minus src1.

  */
void vglDiff(VglImage*  src0, VglImage*  src1, VglImage*  dst);

/** Dilation of image by 3x3 cross structuring element.

  */
void vglDilateCross3(VglImage*  src, VglImage*  dst);

/** Dilation of image by 3x3 square structuring element.

  */
void vglDilateSq3(VglImage*  src, VglImage*  dst);

/** Erosion of image by 3x3 cross structuring element.

  */
void vglErodeCross3(VglImage*  src, VglImage*  dst);

/** Erosion of image by horizontal line with 3 pixels.

  */
void vglErodeHL3(VglImage*  src, VglImage*  dst);

/** Erosion of image by horizontal line with 5 pixels.

  */
void vglErodeHL5(VglImage*  src, VglImage*  dst);

/** Erosion of image by horizontal line with 7 pixels.

  */
void vglErodeHL7(VglImage*  src, VglImage*  dst);

/** Erosion of image by 3x3 square structuring element.

  */
void vglErodeSq3(VglImage*  src, VglImage*  dst);

/** Erosion of image by 3x3 square structuring element. Uses an offset array with 9 elements. Slower than vglErodeSq3.

  */
void vglErodeSq3off(VglImage*  src, VglImage*  dst);

/** Erosion of image by 5x5 square structuring element.

  */
void vglErodeSq5(VglImage*  src, VglImage*  dst);

/** Erosion of image by 3x3 square structuring element. Uses an offset array with 25 elements. Slower than vglErodeSq5.

  */
void vglErodeSq5off(VglImage*  src, VglImage*  dst);

/** Erosion of image by 7x7 square structuring element.

  */
void vglErodeSq7(VglImage*  src, VglImage*  dst);

/** Erosion of image by square structuring element. The parameter "side" is the dimension of the square side in pixels.

  */
void vglErodeSqSide(VglImage*  src, VglImage*  dst, int  side);

/** Erosion of image by vertical line with 3 pixels.

  */
void vglErodeVL3(VglImage*  src, VglImage*  dst);

/** Erosion of image by vertical line with 5 pixels.

  */
void vglErodeVL5(VglImage*  src, VglImage*  dst);

/** Erosion of image by vertical line with 7 pixels.

  */
void vglErodeVL7(VglImage*  src, VglImage*  dst);

/** Feature Points are defined as function of the crossing number
and number of neighbors of a pixel. 

The number of neighbors is indicated as Nb. Crossing number is defined as

Nc = number of occurrences of the pattern 01 in the neighborhood of P

    Neighborhood pixels are indexed as follows:

    P3 P2 P1

    P4 P  P0

    P5 P6 P7


    All the ending points are feature points. Are defined as
Se = { P | Nc(P) = 1 }


    Feature points type 1, denoted as S1, are defined as
S1 = { P | Nc(P) >= 3}

    Feature points type 2, denoted as S2, are defined as
S1 = { P | Nb(P) >= 3}

    Feature points type 3, denoted as S3, are defined as
S3 = { P | Nc(P) >= 3 or Nb(P) >= 4 }


    References:

    Ke Liu et al., Identification of fork points on the skeletons
        of handwritten chinese characters

  */
void vglFeaturePoints(VglImage*  src, VglImage*  dst, int  type);

/** Blurs image by 3x3 square gaussian structuring element.

  */
void vglGaussianBlurSq3(VglImage*  src, VglImage*  dst);

/** Convert image to grayscale by calculating the scalar product of (r, g, b) and (.2125, .7154, .0721).

  */
void vglGray(VglImage*  src, VglImage*  dst);

/** Flip image horizontally i.e. left becomes right.

    Image flip done by shader.
  */
void vglHorizontalFlip(VglImage*  src, VglImage*  dst);

/** vglInOut

    Test and model for IN_OUT semantics

  */
void vglInOut(VglImage*  src, VglImage*  dst);

/** Calculate Julia set

  */
void vglJulia(VglImage*  dst, float  ox= 0.0, float  oy= 0.0, float  half_win= 1.0, float  c_real= -1.36, float  c_imag= .11);

/** Laplacian of image by 3x3 square structuring element.

  */
void vglLaplaceSq3(VglImage*  src, VglImage*  dst);

/** Calculate Mandelbrot set

  */
void vglMandel(VglImage*  dst, float  ox= 0.0, float  oy= 0.0, float  half_win= 1.0);

/** Get specified level of detail.

  */
void vglMipmap(VglImage*  src, VglImage*  dst, float  lod);

/** Multiply image by scalar.

  */
void vglMulScalar(VglImage*  src, VglImage*  dst, float  factor);

/** VglAdd

    Sum of two images.

  */
void vglMultiInput(VglImage*  src0, VglImage*  src1, VglImage*  dst, float  weight= .5);

/** vglGray

    Convert image to grayscale

  */
void vglMultiOutput(VglImage*  src, VglImage*  dst, VglImage*  dst1);

/** Add gaussian noise to image

  */
void vglNoise(VglImage*  src, VglImage*  dst);

/** Inverts image.

  */
void vglNot(VglImage*  src, VglImage*  dst);

/** Logical OR between two images

  */
void vglOr(VglImage*  src0, VglImage*  src1, VglImage*  dst);

/** Rescales corners of image to given corners

  */
void vglRescale(VglImage*  src, VglImage*  dst, float  x0, float  y0, float  x1, float  y1);

/** Converts image RGB to BGR color space

  */
void vglRgbToBgr(VglImage*  src, VglImage*  dst);

/** Converts image RGB to HSL color space

  */
void vglRgbToHsl(VglImage*  src, VglImage*  dst);

/** Converts image RGB to HSV color space

  */
void vglRgbToHsv(VglImage*  src, VglImage*  dst);

/** Converts image RGB to XYZ color space.

  */
void vglRgbToXyz(VglImage*  src, VglImage*  dst);

/** Roberts gradient of image

  */
void vglRobertsGradient(VglImage*  src, VglImage*  dst);

/** Stores in output pixel the sum of 4 adjacent pixels of the input
    image. 
    The width and height of the output image must be half of the input image.

  */
void vglSelfSum22(VglImage*  src, VglImage*  dst);

/** Stores in output pixel the sum of 3 adjacent pixels of the input
    image. 
    The height of the output image must be 1/3th of the input image.

  */
void vglSelfSum3v(VglImage*  src, VglImage*  dst);

/** Stores in output pixel the sum of 4 adjacent pixels of the input
    image. 
    The width of the output image must be 1/4th of the input image.

  */
void vglSelfSum4h(VglImage*  src, VglImage*  dst);

/** Stores in output pixel the sum of 5 adjacent pixels of the input
    image. 
    The width of the output image must be 1/5th of the input image.

  */
void vglSelfSum5h(VglImage*  src, VglImage*  dst);

/** Stores in output pixel the sum of 5 adjacent pixels of the input
    image. 
    The height of the output image must be 1/5th of the input image.

  */
void vglSelfSum5v(VglImage*  src, VglImage*  dst);

/** Sharpens image using 3x3 square window.

  */
void vglSharpenSq3(VglImage*  src, VglImage*  dst);

/** Sobel gradient of image

  */
void vglSobelGradient(VglImage*  src, VglImage*  dst);

/** Sobel edge filtering in X direction.

  */
void vglSobelXSq3(VglImage*  src, VglImage*  dst);

/** Sobel edge filtering in Y direction.

  */
void vglSobelYSq3(VglImage*  src, VglImage*  dst);

/** Sum of two images.

  */
void vglSum(VglImage*  src0, VglImage*  src1, VglImage*  dst);

/** Weighted sum of two images. The first image is multiplied by weight, and the second, by 1 - weight. Default weight is 0.5.

  */
void vglSumWeighted(VglImage*  src0, VglImage*  src1, VglImage*  dst, float  weight= .5);

/** Convert image from RGB to BGR and vice versa.

  */
void vglSwapRGB(VglImage*  src, VglImage*  dst);

/** Test and model for IN_OUT semantics

  */
void vglTestInOut(VglImage*  src_dst, float  r, float  g, float  b, float  a= 0.0);

/** Test and model for IN_OUT semantics, with double output.

  */
void vglTestInOut2(VglImage*  src_dst, VglImage*  dst);

/** Test and model for multiple input functions.

  */
void vglTestMultiInput(VglImage*  src0, VglImage*  src1, VglImage*  dst, float  weight= .5);

/** Test and model for multiple output functions.

  */
void vglTestMultiOutput(VglImage*  src, VglImage*  dst, VglImage*  dst1);

/** vglDilate

    Dilation of image by 3x3 square structuring element.

  */
void vglTeste(VglImage*  src, VglImage*  dst);

/** Return one step of thinning. 
    Algorithm by Bernard and Manzanera 1999.
    Receive as input the image to be thinned and its erosion by a elementary 
cross structuring element.
    Neighborhood pixels are indexed as follows:

\f$
\begin{array}{ccc}

    P3  &  P2  &  P1    \\
    P4  &  P8  &  P0    \\
    P5  &  P6  &  P7

\end{array}
\f$

    References:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    T. M. Bernard and A. Manzanera, Improved low complexity fully parallel
        thinning algorithms, 1999

  */
void vglThinBernardAux(VglImage*  src, VglImage*  eroded, VglImage*  dst);

/** Return one step of thinning.
    Algorithm by Chin, Wan Stover and Iverson, 1987.
    Receive as input the image to be thinned, buffer image and number 
    of times to iterate.
    Neighborhood pixels are indexed as follows:

\f$ 
\begin{array}{ccccc}

    x   & x   & P10 & x   & x    \\

    x   & P3  & P2  & P1  & x    \\

    P11 & P4  & P0  & P8  & P9   \\

    x   & P5  & P6  & P7  & x    \\

    x   & x   & P12 & x   & x

\end{array}
\f$ 

    References:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    R. T. Chin et al., A one-pass thinning algorithm and its parallel 
        implementation, 1987
  */
void vglThinChinAux(VglImage*  src, VglImage*  dst);

/** Threshold of image. If value is greater than threshold, output is top,
    else, output is 0. Default top value is 1.

  */
void vglThresh(VglImage*  src, VglImage*  dst, float  thresh, float  top= 1.0);

/** Threshold of image. If value is equal to level, output is top,
    else, output is 0. Default top value is 1.  
    Use after some Distance Transform to get a single distance level set.

  */
void vglThreshLevelSet(VglImage*  src, VglImage*  dst, float  thresh, float  top= 1.0);

/** Flip image vertically i.e. top becomes bottom.

    Image flip done by shader.
  */
void vglVerticalFlip(VglImage*  src, VglImage*  dst);

/** Finds edge by using a White-Rohrer mask.

  */
void vglWhiteRohrerEdge(VglImage*  src, VglImage*  dst, float  radius);

/** Stores sobel edge filtering in X direction in red channel
    grayscale in y and sobel edge filtering in Y direction in green channel

  */
void vglXGY(VglImage*  src, VglImage*  dst);

/** Zoom image by factor.

  */
void vglZoom(VglImage*  src, VglImage*  dst, float  factor);


/*********************************************************************
***                                                                 ***
***  Source code generated by kernel2cu.pl                          ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/
/** vglCudaCopy

    Copy of image in cuda context.
*/
void vglCudaCopy(VglImage*  input, VglImage*  output);

/** vglCudaInvert

    Inverts image stored in cuda context.
*/
void vglCudaInvert(VglImage*  input, VglImage*  output);

/** vglCudaInvertOnPlace

    Inverts image, stored in cuda context, on place.
*/
void vglCudaInvertOnPlace(VglImage*  input);


#endif

