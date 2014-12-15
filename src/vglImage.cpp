//#include <sh/sh.hpp>
#include <iostream>

//GL
#include <GL/glew.h>
#include <GL/freeglut.h>


//IplImage, cvLoadImage
//#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
//#include <cvaux.h>

#include "vglContext.h"
#include "vglImage.h"
#include "vglLoadShader.h"

//vglDilateSq3, vglErodeSq3
#include "glsl2cpp_shaders.h"

//uint8_t 16 32 64
#include <stdint.h>

  /** \brief Refresh all output images.
   */
  void VglNamedWindowList::RefreshAll(int split){
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    Reset();
    int curr_id = Cycle();
    if (split < VGL_MIN_WINDOW_SPLIT || split > VGL_MAX_WINDOW_SPLIT){
      printf("VglNamedWindowList::RefreshAll(): Error: split = %d not between %d and %d. Using default, %d",
	     split, VGL_MIN_WINDOW_SPLIT, VGL_MAX_WINDOW_SPLIT, VGL_DEFAULT_WINDOW_SPLIT);
      split = VGL_DEFAULT_WINDOW_SPLIT;
    }
    while (curr_id >= 0){
      Refresh(curr_id, split);
      curr_id = Cycle();
    }
    glutSwapBuffers();
  }

  /** \brief Refresh single output image.
   */
void VglNamedWindowList::Refresh(int win_index, int split){
    float x0, x1, y0, y1;
    int win_x, win_y;
    //printf("VglNamedWindowList::Refresh(%d)\n", win_index);

    vglCheckContext(WindowList[win_index].image, VGL_GL_CONTEXT);

    if (split < VGL_MIN_WINDOW_SPLIT || split > VGL_MAX_WINDOW_SPLIT){
      printf("VglNamedWindowList::Refresh(): Error: split = %d not between %d and %d. Using default, %d",
	     split, VGL_MIN_WINDOW_SPLIT, VGL_MAX_WINDOW_SPLIT, VGL_DEFAULT_WINDOW_SPLIT);
      split = VGL_DEFAULT_WINDOW_SPLIT;
    }

    if (!FreePosition[win_index]){
      glutSetWindow(main_window_id);
      if (!WindowList[win_index].image){
        printf("VglNamedWindowList::Refresh(%d): Warning: image not set\n", win_index);
        return;
      }

      win_x = win_index % split;
      win_y = win_index / split;

      x0 = VGL_WIN_X0 + ((float)win_x      ) * VGL_WIN_DX / (float)split;
      x1 = VGL_WIN_X0 + ((float)win_x + 1.0) * VGL_WIN_DX / (float)split;
      y0 = VGL_WIN_Y1 - ((float)win_y      ) * VGL_WIN_DY / (float)split;
      y1 = VGL_WIN_Y1 - ((float)win_y + 1.0) * VGL_WIN_DY / (float)split;

      //glColor3f((float)win_index/2.0, 1.0 - (float)win_index/2.0, 0.0);
      glBindTexture(GL_TEXTURE_2D, WindowList[win_index].image->tex);
      //printf("VglNamedWindowList::Refresh(%d) Binding texture %d\n", win_index, WindowList[win_index].image->tex);
        
      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
        //glVertex3f ( -1.0, -1.0, 0.0);
          glVertex3f (   x0,   y0, 0.0);

          glTexCoord2f( 1.0,  0.0);
        //glVertex3f (  1.0, -1.0, 0.0);
          glVertex3f (   x1,   y0, 0.0);

          glTexCoord2f( 1.0,  1.0);
        //glVertex3f (  1.0,  1.0, 0.0);
          glVertex3f (   x1,   y1, 0.0);

          glTexCoord2f( 0.0,  1.0);
        //glVertex3f ( -1.0,  1.0, 0.0);
          glVertex3f (   x0,   y1, 0.0);
      glEnd();
      //glutSwapBuffers();
    }
  }

  void VglNamedWindowList::Reset(void){
    for (int i = 0; i < VGL_MAX_WINDOWS; i++){
      if (!FreePosition[i]){
        current_window = -1;
      }
    }    
    //printf("VglNamedWindowList::Reset: current_window = %d\n", current_window);
  }

  int VglNamedWindowList::Cycle(void){
    int i;
    for (i = current_window + 1; i < VGL_MAX_WINDOWS; i++){
      if (!FreePosition[i]){
        current_window = i;
        break;
      }
    }
    if (i == VGL_MAX_WINDOWS){
      current_window = -1;
    }
    //printf("VglNamedWindowList::Cycle: current_window = %d\n", current_window);
    return current_window;
  }

  VglNamedWindowList::VglNamedWindowList(void){
    for (int i = 0; i < VGL_MAX_WINDOWS; i++){
      FreePosition[i] = 1;
    }
  }

  VglNamedWindowList::~VglNamedWindowList(void){

  }

  int VglNamedWindowList::MaxWindows(void){
    return VGL_MAX_WINDOWS;
  }

  int VglNamedWindowList::CreateNamedSubwindow(char* name){
    for (int i = 0; i < VGL_MAX_WINDOWS; i++){
      if (FreePosition[i]){
        WindowList[i].name = name;
        FreePosition[i] = 0;
        return i;
      }
    }
    return -1;
  }

  int VglNamedWindowList::ShowImage(char* name, VglImage* image){
    int win_id = WindowIdByName(name);
    if (win_id >= 0){
      WindowList[win_id].image = image;
      printf("VglNamedWindowList::ShowImage: win_id = %d image = %p\n", win_id, image);
    }
    else{
      printf("VglNamedWindowList::ShowImage: window %s not found\n", name);
    }
    return win_id;
  }

  int VglNamedWindowList::WindowIdByName(char* name){
    for (int i = 0; i < VGL_MAX_WINDOWS; i++){
      if (!FreePosition[i]){
        int retval = strcmp(name, WindowList[i].name);
        if (retval == 0){
          return i;
	}
      }
    }
    return -1;
  }

/*
  void VglNamedWindowList::vglShowImage(char* winname, VglImage* image)
  {
    window_list->ShowImage(winname, image);
  }
*/

int VglNamedWindowList::NamedWindow(char* winname){
    return CreateNamedSubwindow(winname);
}


////////// VglImage functions

/** Initialize GLUT and create output window with default size (1280, 960).
  */
int vglInit()
{
  return vglInit(1280, 960);
}

/** Initialize GLUT and create output window with size (w, h).
  */
int vglInit(int w, int h)
{
    char* argv = new char[255];  
    int argc = 0;
    int window_id = 0;

    int glut_time = glutGet(GLUT_ELAPSED_TIME);
    printf("Glut elapsed time = %dms\n", glut_time);
    static int started = 0;
    if(!started)
    {
      glutInit(&argc, &argv);
      glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
      glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
      glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
      glutInitWindowSize(w, h);
      if (w < 10 && h < 10)
      {
        glutInitWindowPosition(-50, -50);
      }
      else
      {
        glutInitWindowPosition(-50, -50);
      }
      window_id = glutCreateWindow("Main window");

      glEnable(GL_TEXTURE_2D); //deve ficar depois de glutCreateWindow

      GLenum err = glewInit();
      if (GLEW_VERSION_1_3)
      {
        if(!glewIsSupported("GL_EXT_framebuffer_object"))
          fprintf(stderr, "%s: %s: glGenFramebuffersEXT not supported. The program may not work.\n", __FILE__, __FUNCTION__);

      }
      else
      {
        fprintf(stderr, "%s: %s: OpenGL 1.3 not supported. The program may not work.\n", __FILE__, __FUNCTION__);
      }
 
      started = 1;

      //glutDisplayFunc(display);
      //glutIdleFunc(display);
      //glutReshapeFunc(reshape);
      //glutKeyboardFunc(keyboard);
    }
    return window_id;
}

/** Send image data from RAM to GPU. Add GPU as valid context.

    If swapRGB is true, channels R and B are swapped.
  */
void vglUpload(VglImage* image, int swapRGB){
  IplImage* ipl = image->ipl;
  const int LEVEL = 0;
  GLenum glFormat;
  GLenum glTarget;
  GLenum glType;
  GLenum internalFormat;
  int depth      = image->depth;
  int nChannels  = image->nChannels;
  int ndim       = image->ndim;
  int has_mipmap = image->has_mipmap;


  //printf("vglUpload: image context = %d\n", image->inContext);
  if (!vglIsInContext(image, VGL_RAM_CONTEXT)  &&
      !vglIsInContext(image, VGL_BLANK_CONTEXT)    ){
#ifdef __DEBUG__
    fprintf(stderr, "vglUpload: Error: image context = %d not in VGL_RAM_CONTEXT or VGL_BLANK_CONTEXT\n", image->inContext);
#endif
    return;
  }

  if (nChannels == 3){
        //swapRGB = (swapRGB + 1) % 2;
	if(ndim == 3)
	{
		vglNdarray3To4Channels(image);
	}
	else
	{
		/*IplImage* iplRGBA = cvCreateImage(cvGetSize(image->ipl), depth, 4);
		printf("\t\t\t    ipl->nChannels = %d\n", image->ipl->nChannels);
		printf("\t\t\tiplRGBA->nChannels = %d\n", iplRGBA->nChannels);
		cvCvtColor(image->ipl, iplRGBA, CV_BGR2RGBA);
		cvReleaseImage(&(image->ipl));
		free(image->ipl);
		image->ipl = iplRGBA;
		image->nChannels = 4;
		printf("\t\t\tipl->nChannels = %d\n", image->ipl->nChannels);
		nChannels = 4;*/
	}
  }

  if (nChannels == 3){
    if (swapRGB){
      glFormat = GL_RGB;
    }
    else{
      glFormat = GL_BGR;
    }
  }
  else if (nChannels == 4)
  {
    if (swapRGB){
      glFormat = GL_RGBA;
    }
    else{
      glFormat = GL_BGRA;
    }
  }
  else{
    glFormat = GL_LUMINANCE;
  }

  //vglInit(1, 1);
  if (image->tex == -1){
    glGenTextures(1, &image->tex);
  }

  if (depth != IPL_DEPTH_8U){
    fprintf(stderr, "%s: %s: Pixel type not supported. Must be IPL_DEPTH_8U\n", __FILE__, __FUNCTION__);
  }

  if (ndim <= 0){
    fprintf(stderr, "vglUpload: Image ndim (depth) must be greater than zero. Assuming ndim = 2\n");
    ndim =2;
  }

  if (ndim == 2){
    glTarget = GL_TEXTURE_2D;
  }
  else if (ndim == 3){
    glTarget = GL_TEXTURE_3D;
  }
  else {
    fprintf(stderr, "%s: %s: Error: images with more than 3 dimensions not supported\n", __FILE__, __FUNCTION__);
  }

  //printf("%s:%s: w x h x d = %d x %d x %d\n", __FILE__, __FUNCTION__, image->shape[0], image->shape[1], image->shape[2]);

  glBindTexture(glTarget, image->tex);
  glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);

  ERRCHECK()

  // MIPMAPPING!!!!
  if (has_mipmap){
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  }
  ERRCHECK()

  switch (depth){
          case IPL_DEPTH_8U:  glType = GL_UNSIGNED_BYTE;  break; 
          case IPL_DEPTH_16U: glType = GL_UNSIGNED_SHORT; break; 
          case IPL_DEPTH_32F: glType = GL_FLOAT; break; 
          case IPL_DEPTH_8S:  glType = GL_BYTE; exit(1);  break; 
          case IPL_DEPTH_16S: glType = GL_SHORT; exit(1); break; 
          case IPL_DEPTH_32S: glType = GL_INT; break; 
          case IPL_DEPTH_1U:
          default: 
            fprintf(stderr, "vglUpload: Error: uploading unsupported image depth\n");
            vglPrintImageInfo(image);
            exit(1);
  }

  if (nChannels >= 3){
    if (glType == GL_FLOAT){
        internalFormat = GL_RGBA32F_ARB;
    }
    else{
        internalFormat = GL_RGBA;
    }
  }
  else{
        internalFormat = GL_RED; //Must be fixed, but for now, it's the fix.
  }

  if (ndim == 3){
    glTexImage3D(glTarget, LEVEL, internalFormat, 
                 image->shape[VGL_WIDTH], image->shape[VGL_HEIGHT], image->shape[VGL_LENGTH], 0,
                 glFormat, glType, image->ndarray);
  }
  else{
    glTexImage2D(glTarget, LEVEL, internalFormat, 
                 ipl->width, ipl->height, 0,
                 glFormat, glType, ipl->imageData);
  }

  //printf("%s:%s: checking framebuffer...\n", __FILE__, __FUNCTION__);

  CHECK_FRAMEBUFFER_STATUS()
  ERRCHECK()

  if (image->fbo == -1){
    glGenFramebuffersEXT(1, &image->fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);
    if (glTarget == GL_TEXTURE_3D){
      int layer = 0; // will render only to layer 0 
      glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT, 
				glTarget, image->tex, 0, layer);
    }
    else{
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT, 
                              glTarget, image->tex, 0);
    }
    CHECK_FRAMEBUFFER_STATUS()
    ERRCHECK()
  }

  if (vglIsInContext(image, VGL_RAM_CONTEXT)){
    vglAddContext(image, VGL_GL_CONTEXT);
  }

}

/* * Clones image with same contents and dimensions as parameter

    Same as vglCopyCreateImage
 */
/*
VglImage* vglCloneImage(IplImage* img_in, int ndim /*=2* /, int has_mipmap /*=0* /)
{
  VglImage* vglImage = new VglImage;
  if (!img_in){
    fprintf(stderr, "vglCloneImage: Error: img_in is null\n");
    return 0;
  }
  vglImage->ipl = cvCloneImage(img_in);
  if (!vglImage->ipl){
    fprintf(stderr, "vglCloneImage: Error allocating memory for vglImage->ipl field\n");
    return 0;
  }
  vglImage->width     = img_in->width;
  vglImage->height    = img_in->height;
  vglImage->ndim      = ndim;
  vglImage->depth     = img_in->depth;
  vglImage->nChannels = img_in->nChannels;
  vglImage->has_mipmap = has_mipmap;
  vglImage->fbo = -1;
  vglImage->tex = -1;
  vglImage->cudaPtr = NULL;
  vglImage->cudaPbo = -1;
  fprintf(stderr, "vglCloneImage: ndim = %d\n", vglImage->ndim);

  vglSetContext(vglImage, VGL_RAM_CONTEXT);
  vglUpload(vglImage);

  if (vglImage->ipl){
    return vglImage;
  }
  else{
    free(vglImage);
    return 0;
  }
}
*/

/** Create image with same format and data as img_in.

 */
VglImage* vglCopyCreateImage(VglImage* img_in)
{
  VglImage* retval = vglCreateImage(cvSize(img_in->shape[VGL_WIDTH], img_in->shape[VGL_HEIGHT]), img_in->depth, img_in->nChannels, img_in->ndim, img_in->has_mipmap);
  vglCopy(img_in, retval);
  return retval;
}

/** Create image with same format and data as img_in
 */
VglImage* vglCopyCreateImage(IplImage* img_in, int ndim /*=2*/, int has_mipmap /*=0*/)
{
  VglImage* retval = vglCreateImage(cvSize(img_in->width, img_in->height), img_in->depth, img_in->nChannels, ndim, has_mipmap);
  cvCopy(img_in, retval->ipl);
  vglSetContext(retval, VGL_RAM_CONTEXT);
  vglUpload(retval);
  return retval;
}

/** Create image with same format as img_in
 */
VglImage* vglCreateImage(VglImage* img_in)
{
  if (img_in->ndim == 2)
  {
    return vglCreateImage(cvSize(img_in->shape[VGL_WIDTH], img_in->shape[VGL_HEIGHT]), img_in->depth, img_in->nChannels, img_in->ndim, img_in->has_mipmap);
  }
  else
  {
    return vglCreate3dImage(cvSize(img_in->shape[VGL_WIDTH], img_in->shape[VGL_HEIGHT]), img_in->depth, img_in->nChannels, img_in->shape[VGL_LENGTH], img_in->has_mipmap);
  }
}


/** Create image with same format as img_in
 */
VglImage* vglCreateImage(IplImage* img_in, int ndim /*=2*/, int has_mipmap /*=0*/)
{
  return vglCreateImage(cvGetSize(img_in), img_in->depth, img_in->nChannels, ndim, has_mipmap);

}

/** Create image as described by the parameters
 */
VglImage* vglCreateImage(CvSize size, int depth, int nChannels, int ndim, int has_mipmap)
{
  VglImage* vglImage = new VglImage;
  IplImage* ipl = cvCreateImage(size, depth, nChannels);
  if (!ipl){
    fprintf(stderr, "vglCreateImage: Error creating vglImage->ipl field\n");
    free(vglImage);
    return 0;
  }
  for (int i = 0; i < VGL_MAX_DIM; i++)
  {
    vglImage->shape[i] = 0;
  }
  
  vglImage->ipl = ipl;
  vglImage->ndarray = NULL;
  vglImage->shape[VGL_WIDTH] = ipl->width;
  vglImage->shape[VGL_HEIGHT] = ipl->height;
  vglImage->ndim      = ndim;
  vglImage->depth     = ipl->depth;
  vglImage->nChannels = ipl->nChannels;
  vglImage->has_mipmap = has_mipmap;
  vglImage->fbo = -1;
  vglImage->tex = -1;
  vglImage->cudaPtr = NULL;
  vglImage->cudaPbo = -1;
#ifdef __OPENCL__
  vglImage->oclPtr = NULL;
#endif
  vglImage->filename = NULL;

  vglSetContext(vglImage, VGL_BLANK_CONTEXT);
  vglUpload(vglImage);
  
  return vglImage;
}

/** Create image as described by the parameters
 */
VglImage* vglCreate3dImage(CvSize size, int depth, int nChannels, int layers, int has_mipmap /*=0*/)
{
  VglImage* vglImage = new VglImage;
  IplImage* ipl = NULL;

  for (int i = 0; i < VGL_MAX_DIM; i++)
  {
    vglImage->shape[i] = 0;
  }
  
  vglImage->ipl = ipl;
  vglImage->ndarray = NULL;
  vglImage->shape[VGL_WIDTH] = size.width;
  vglImage->shape[VGL_HEIGHT] = size.height;
  vglImage->shape[VGL_LENGTH] = layers;
  vglImage->ndim      = 3;
  vglImage->depth     = depth;
  vglImage->nChannels = nChannels;
  vglImage->has_mipmap = has_mipmap;
  vglImage->fbo = -1;
  vglImage->tex = -1;
  vglImage->cudaPtr = NULL;
  vglImage->cudaPbo = -1;
#ifdef __OPENCL__
  vglImage->oclPtr = NULL;
#endif
  vglImage->filename = NULL;


  int bytesPerPixel = vglImage->depth / 8;
  if (bytesPerPixel < 1) bytesPerPixel = 1;

  int c = 1;
  if (nChannels >= 3)
  {
    c = 4; // Although image may be RGB, a fourth channel is allocated, as needed by OpenCL.
  }

  vglImage->ndarray = malloc(size.width*size.height*c*layers*bytesPerPixel);

  vglSetContext(vglImage, VGL_BLANK_CONTEXT);
  //vglUpload(vglImage);

  return vglImage;
}


/** Save PGM 3d images on the disk
*/
void vglSave3dImage(VglImage* image, char* filename, int lStart, int lEnd)
{
	//vglDownload(image); //must be fixed before enabling
        char* temp_filename = (char*)malloc(strlen(filename)+256);
        sprintf(temp_filename, filename, lStart);
        int d = image->depth / 8;
        if (d < 1) d = 1; //d is the byte size of the depth color format

        char* temp_image = (char*)malloc(image->shape[VGL_HEIGHT]*image->shape[VGL_WIDTH]*image->nChannels*d);
        memcpy(temp_image,image->ndarray,image->shape[VGL_HEIGHT]*image->shape[VGL_WIDTH]*image->nChannels*d);

        IplImage* ipl = cvCreateImage(cvSize(image->shape[VGL_WIDTH], image->shape[VGL_HEIGHT]), image->depth, image->nChannels);
        ipl->imageData = temp_image;

        cvSaveImage(temp_filename,ipl);
        int c = image->shape[VGL_HEIGHT]*image->shape[VGL_WIDTH]*d*image->nChannels;
        for(int i = lStart+1; i <= lEnd; i++)
        {
                memcpy(temp_image,((char*)image->ndarray)+c,image->shape[VGL_HEIGHT]*image->shape[VGL_WIDTH]*image->nChannels*d);
                ipl->imageData = temp_image;
                sprintf(temp_filename, filename, i);
                cvSaveImage(temp_filename, ipl);
                c += image->shape[VGL_HEIGHT]*image->shape[VGL_WIDTH]*image->nChannels*d;
        }
}                

/** Converts ndarray from 3 channels to 4 channels
  */
void vglNdarray3To4Channels(VglImage* img)
{

    if (img->nChannels == 4)
    {
        fprintf(stdout, "%s:%s: Warning: image already has 4 channels\n", __FILE__, __FUNCTION__);
        return;
    }

    int d = img->depth / 8;
    if (d < 1) d = 1; //d is the byte size of the depth color format
        
    int datasize = img->shape[VGL_HEIGHT] * img->shape[VGL_WIDTH] * 4 * d * img->shape[VGL_LENGTH];

    void* newndarray = (char*)malloc(img->shape[VGL_HEIGHT] * img->shape[VGL_WIDTH] * 4 * d * img->shape[VGL_LENGTH]);

    int offset = 0;
    uint8_t temp_alpha = 0;
    for(int i = 0; i < (datasize/d); i++)//for(int i = (datasize/d)-1; i >= 0; i--)
    {
        if (((i+1) % 4) == 0)
        {
            switch(d)
            {
                case 1:
                    ((uint8_t*)newndarray)[i] = temp_alpha;
                    break;
                case 2:
                    ((uint16_t*)newndarray)[i] = temp_alpha;
                    break;
                case 4:
                    ((uint32_t*)newndarray)[i] = temp_alpha;
                    break;
                case 8:
                  ((uint64_t*)newndarray)[i] = temp_alpha;
                    break;
            }
        }
        else
        {
            switch(d)
            {
                case 1:
                    ((uint8_t*)newndarray)[i] = ((uint8_t*)img->ndarray)[offset];
                    break;
                case 2:
                    ((uint16_t*)newndarray)[i] = ((uint16_t*)img->ndarray)[offset];
                    break;
                case 4:
                    ((uint32_t*)newndarray)[i] = ((uint32_t*)img->ndarray)[offset];
                    break;
                case 8:
                    ((uint64_t*)newndarray)[i] = ((uint64_t*)img->ndarray)[offset];
                    break;
            }
            offset++;
        }
    }
    free(img->ndarray);

    img->ndarray = newndarray;
    img->nChannels = 4;
}

/** Converts ndarray from 4 channels to 3 channels
  */
void vglNdarray4To3Channels(VglImage* img)
{
    if (img->nChannels == 3)
    {
        fprintf(stdout, "%s:%s: Warning: image already has 3 channels\n", __FILE__, __FUNCTION__);
        return;
    }

    int d = img->depth / 8;
    if (d < 1) d = 1;
        
    int datasize = img->shape[VGL_HEIGHT] * img->shape[VGL_WIDTH] * img->nChannels * d * img->shape[VGL_LENGTH];

    void* newndarray = malloc(img->shape[VGL_HEIGHT] * img->shape[VGL_WIDTH] * 3 * d * img->shape[VGL_LENGTH]);

    int offset = 0;
    uint8_t temp_alpha = 0;
    for(int i = 0 ; i < (datasize/d); i++)
    {
        if (((i+1) % 4) != 0)
        {
            switch(d)
            {
               case 1:
                   ((uint8_t*)newndarray)[offset] = ((uint8_t*)img->ndarray)[i];
                   break;
               case 2:
                   ((uint16_t*)newndarray)[offset] = ((uint16_t*)img->ndarray)[i];
                   break;
               case 4:
                   ((uint32_t*)newndarray)[offset] = ((uint32_t*)img->ndarray)[i];
                   break;
               case 8:
                   ((uint64_t*)newndarray)[offset] = ((uint64_t*)img->ndarray)[i];
                   break;
            }
            offset++;
        }
    }

    free(img->ndarray);

    img->ndarray = newndarray;
    img->nChannels = 3;
}

/** Convert ipl field of VglImage from 3 to 4 channels
 */
void vglIpl3To4Channels(VglImage* img)
{
    if (!img->ipl){
        return;
    }
    if (img->ipl->nChannels != 3)
    {
        return;
    }
  
    IplImage* iplRGBA = cvCreateImage(cvGetSize(img->ipl), img->ipl->depth, 4);
    cvCvtColor(img->ipl, iplRGBA, CV_RGB2RGBA);
    cvReleaseImage(&(img->ipl));
    img->ipl = iplRGBA;
    img->nChannels = 4;
}

/** Convert ipl field of VglImage from 4 to 3 channels
 */
void vglIpl4To3Channels(VglImage* img)
{
    if (!img->ipl){
        return;
    }
    if (img->ipl->nChannels != 4)
    {
        return;
    }
  
    IplImage* iplRGB = cvCreateImage(cvGetSize(img->ipl), img->ipl->depth, 3);
    cvCvtColor(img->ipl, iplRGB, CV_RGBA2RGB);
    cvReleaseImage(&(img->ipl));
    img->ipl = iplRGB;
    img->nChannels = 4;
}

/** Convert VglImage from 3 to 4 channels
 */
void vglImage3To4Channels(VglImage* img)
{
    if (img->nChannels != 3)
    {
        return;
    }
    if (img->ipl)
    {
        vglIpl3To4Channels(img);
    }
    if (img->ndarray)
    {
        vglNdarray3To4Channels(img);
    }
}

/** Convert VglImage from 4 to 3 channels
 */
void vglImage4To3Channels(VglImage* img)
{
    if (img->nChannels != 4)
    {
        return;
    }
    if (img->ipl)
    {
        vglIpl4To3Channels(img);
    }
    if (img->ndarray)
    {
        vglNdarray4To3Channels(img);
    }
}

/** Release memory occupied by image in RAM and GPU
 */
void vglReleaseImage(VglImage** p_image)
{
  VglImage* image = *p_image;
  if (!image){
    fprintf(stdout, "%s:%s: Warning: image is null\n", __FILE__, __FUNCTION__);
    return;
  }
  if (image->ipl){
    cvReleaseImage(&(image->ipl));
  }
  if (image->ndarray){
    free(image->ndarray);
  }
  if (image->fbo != -1){
    glDeleteFramebuffersEXT(1, &image->fbo); 
  }
  if (image->tex != -1){
    glDeleteTextures(1, &image->tex);
  }
  delete(*p_image);
}

/** Replace IplImage, stored inside a VglImage, with new IplImage.

    Both new and old images must have exactly the same properties,
    dimensions, depth, type etc. Is useful when grabbing frames from a camera.
 */
void vglReplaceIpl(VglImage* image, IplImage* new_ipl)
{
  IplImage* ipl;

  if (!image){
    fprintf(stdout, "vglReplaceIpl: Warning: VglImage is null\n");
    return;
  }
  if (!new_ipl){
    fprintf(stdout, "vglReplaceIpl: Warning: new IplImage is null\n");
    return;
  }
  ipl = image->ipl;
  if (ipl){
    cvReleaseImage(&ipl);
  }
  image->ipl = new_ipl;

  vglSetContext(image, VGL_RAM_CONTEXT);
  vglUpload(image);
}

/** Force transfer of image from GPU to RAM. Add RAM as valid context.

    Transfer done by glReadPixels. Color order is compatible with iplImage, that is, BGR.

    Time to transfer a VGA image = 1.0 to 1.5ms
 */ 
void vglDownloadFaster(VglImage* image/*, VglImage* aux*/){
  IplImage* ipl = image->ipl;
  GLenum glFormat;
  GLenum glType;
  int depth     = image->depth;
  int nChannels = image->nChannels;

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  if (!vglIsInContext(image, VGL_GL_CONTEXT)){
    fprintf(stderr, "vglUpload: Error: image context = %d is not in VGL_GL_CONTEXT\n", image->inContext);
    return;
  }

  glBindTexture(GL_TEXTURE_2D, image->tex);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

  // New version ddantas 5/2/3009
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);
  glViewport(0, 0, 2*image->shape[VGL_WIDTH], 2*image->shape[VGL_HEIGHT]);
  // Old version needs aux image
  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, aux->fbo);
  //glViewport(0, 0, 2*aux->width, 2*aux->height);
  
  ERRCHECK()
      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();

  switch (depth){
          case IPL_DEPTH_8U:  glType = GL_UNSIGNED_BYTE;  break; 
          case IPL_DEPTH_16U: glType = GL_UNSIGNED_SHORT; break; 
          case IPL_DEPTH_32F: glType = GL_FLOAT; break; 
          case IPL_DEPTH_8S:  glType = GL_BYTE;  break; 
          case IPL_DEPTH_16S: glType = GL_SHORT; break; 
          case IPL_DEPTH_32S: glType = GL_INT; break; 
          case IPL_DEPTH_1U:
          default: 
            fprintf(stderr, "vglDownload: Error: downloading unsupported image depth\n");
            vglPrintImageInfo(image);
            exit(1);
  }

  ERRCHECK()

  if (nChannels == 3){
    glFormat = GL_BGR;
  }
  else{
    glFormat = GL_LUMINANCE;
  }

  glReadPixels(0,0,ipl->width,ipl->height, glFormat, glType, ipl->imageData);

  ERRCHECK()

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Force transfer of image from GPU to RAM. Add RAM as valid context.

    Transfer done by glGetTexImage. Color order is compatible with iplImage, that is, BGR.

    Time to transfer a VGA image = 2.5ms
 */ 
void vglDownload(VglImage* image){
  IplImage* ipl = image->ipl;
  GLenum glFormat;
  GLenum glType;
  int ndim       = image->ndim;
  int depth      = image->depth;
  int nChannels  = image->nChannels;

  //glPixelStorei(GL_PACK_ALIGNMENT, 4);


  switch (depth){
          case IPL_DEPTH_8U:  glType = GL_UNSIGNED_BYTE;  break; 
          case IPL_DEPTH_16U: glType = GL_UNSIGNED_SHORT; break; 
          case IPL_DEPTH_32F: glType = GL_FLOAT; break; 
          case IPL_DEPTH_8S:  glType = GL_BYTE;  break; 
          case IPL_DEPTH_16S: glType = GL_SHORT; break; 
          case IPL_DEPTH_32S: glType = GL_INT; break; 
          case IPL_DEPTH_1U:
          default: 
            fprintf(stderr, "vglDownload: Error: downloading unsupported image depth\n");
            vglPrintImageInfo(image);
            exit(1);
  }

  ERRCHECK()

  if (nChannels == 3){
    glFormat = GL_BGR;
  }
  else{
    glFormat = GL_LUMINANCE;
  }

  if (ndim == 2){
    glBindTexture(GL_TEXTURE_2D, image->tex);
    ERRCHECK()
    glGetTexImage(GL_TEXTURE_2D, 0, glFormat, glType, ipl->imageData);
    ERRCHECK()
  }
  else if (ndim == 3){
    glBindTexture(GL_TEXTURE_3D, image->tex);
    ERRCHECK()
    glGetTexImage(GL_TEXTURE_3D, 0, glFormat, glType, image->ndarray);
    ERRCHECK()
  }
  else {
    fprintf(stderr, "%s: %s: Error: images with more than 3 dimensions not supported\n", __FILE__, __FUNCTION__);
  }


  //glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, ipl->imageData);

  ERRCHECK()

  vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Force transfer of image from FBO to RAM. Add RAM as valid context.

    Transfer done by glReadPixels. Color order is compatible with iplImage, that is, BGR.

 */ 
void vglDownloadFBO(VglImage* image){
  IplImage* ipl = image->ipl;
  GLenum glFormat;
  GLenum glType;
  int depth = image->depth;
  int nChannels = image->nChannels;

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);

  switch (depth){
          case IPL_DEPTH_8U:  glType = GL_UNSIGNED_BYTE;  break; 
          case IPL_DEPTH_16U: glType = GL_UNSIGNED_SHORT; break; 
          case IPL_DEPTH_32F: glType = GL_FLOAT; break; 
          case IPL_DEPTH_8S:  glType = GL_BYTE;  break; 
          case IPL_DEPTH_16S: glType = GL_SHORT; break; 
          case IPL_DEPTH_32S: glType = GL_INT; break; 
          case IPL_DEPTH_1U:
          default: 
            fprintf(stderr, "vglDownload: Error: downloading unsupported image depth\n");
            vglPrintImageInfo(image);
            exit(1);
  }

  if (nChannels == 3){
    glFormat = GL_BGR;
  }
  else{
    glFormat = GL_LUMINANCE;
  }

  glReadPixels(0, 0, image->shape[VGL_WIDTH], image->shape[VGL_HEIGHT], glFormat, glType, image->ipl->imageData);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Transfer image from GPU to RAM in format suitable for saving as PPM.

    Use it imediately before vglSavePPM. It is different from 
    vglDownload in two points. The unpack alignment is 1 and 
    color order is RGB

    Time to transfer a VGA image = 3ms
 */ 
void vglDownloadPPM(VglImage* image){
  IplImage* ipl = image->ipl;
  GLint pack;

  glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  ERRCHECK()

  glBindTexture(GL_TEXTURE_2D, image->tex);
  ERRCHECK()
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, ipl->imageData);
  ERRCHECK()

  glPixelStorei(GL_PACK_ALIGNMENT, pack);
  ERRCHECK()

  //This image can not be uploaded correctly and call no vglAddContext.
  //vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Transfer image from GPU to RAM in format suitable for saving as PGM.

    Use it imediately before vglSavePGM. It is different from 
    vglDownload in two points. The unpack alignment is 1 and 
    color is grayscale

    Time to transfer a VGA image = 10ms
 */ 
void vglDownloadPGM(VglImage* image){
  IplImage* ipl = image->ipl;
  GLint pack;

  glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  ERRCHECK()

  glBindTexture(GL_TEXTURE_2D, image->tex);
  ERRCHECK()
  glGetTexImage(GL_TEXTURE_2D, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, ipl->imageData);
  ERRCHECK()

  glPixelStorei(GL_PACK_ALIGNMENT, pack);
  ERRCHECK()

  //This image can not be uploaded correctly and call no vglAddContext.
  //vglAddContext(image, VGL_RAM_CONTEXT);
}

/** Load image from file to new VglImage.

    This function uses cvLoadImage to read image file.
 */
VglImage* vglLoadImage(char* filename, int iscolor, int has_mipmap)
{
  VglImage* img = new VglImage;
  IplImage* ipl = cvLoadImage(filename, iscolor);
  if (!ipl){
    fprintf(stderr, "vglCreateImage: Error loading image from file %s\n", filename);
    free(img);
    return 0;
  }
  img->ipl = ipl;
  img->ndarray = NULL;
  img->shape[VGL_WIDTH]     = ipl->width;
  img->shape[VGL_HEIGHT]    = ipl->height;
  img->ndim      = 2;
  img->depth     = ipl->depth;
  img->nChannels = ipl->nChannels;
  img->has_mipmap = has_mipmap;
  img->fbo = -1;
  img->tex = -1;
  img->cudaPtr = NULL;
  img->cudaPbo = -1;
#ifdef __OPENCL__
  img->oclPtr = NULL;
#endif
  img->filename = NULL;

  vglSetContext(img, VGL_RAM_CONTEXT);
  vglUpload(img);

  if (img->ipl){
    return img;
  }
  else{
    free(img);
    return 0;
  }

}

/** /brief Load sequence of images as 3d image.

    Filename must have a printf compatible integer format specifier, like %d or %03d.
*/
VglImage* vglLoad3dImage(char* filename, int lStart, int lEnd, bool has_mipmap /*=0*/)
{
  VglImage* img = new VglImage;
  char* tempFilename = (char*)malloc(strlen(filename) + 256);
  sprintf(tempFilename, filename, lStart);
  IplImage* ipl = cvLoadImage(tempFilename, CV_LOAD_IMAGE_UNCHANGED);

  if (!ipl){
    fprintf(stderr, "%s: %s: Error loading image %s\n", __FILE__, __FUNCTION__, tempFilename);
    free(img);
    return 0;
  }

  int n = lEnd-lStart+1;
  img->ipl = ipl;
  img->ndarray = NULL;
  img->shape[VGL_WIDTH] = ipl->width;
  img->shape[VGL_HEIGHT] = ipl->height;
  img->shape[VGL_LENGTH] = n;
  img->ndim      = 3;
  img->depth     = ipl->depth;
  img->nChannels = ipl->nChannels;
  img->has_mipmap = has_mipmap;
  img->fbo = -1;
  img->tex = -1;
  img->cudaPtr = NULL;
  img->cudaPbo = -1;
#ifdef __OPENCL__
  img->oclPtr = NULL;
#endif
  img->filename = NULL;

  int d = img->depth / 8;
  if (d < 1) d = 1; //d is the byte size of the depth color format

  img->ndarray = (char*)malloc(ipl->height*ipl->width*ipl->nChannels*d*img->shape[VGL_LENGTH]);

  
  memcpy(img->ndarray,(void*) ipl->imageData,ipl->height*ipl->width*ipl->nChannels*d);
  ipl->imageData = (char*)img->ndarray;
  int c = ipl->height*ipl->width*d*ipl->nChannels;
  for(int i = lStart+1; i <= lEnd; i++)
  {
    sprintf(tempFilename,filename,i);
    ipl = cvLoadImage(tempFilename, CV_LOAD_IMAGE_UNCHANGED);
    if (!ipl){
      fprintf(stderr, "%s: %s: Error loading image %s\n", __FILE__, __FUNCTION__, tempFilename);
      fprintf(stderr, "vglCreateImage: Error creating img->ipl field\n");
      free(img);
      return 0;
    }

    memcpy(((char*)img->ndarray)+c,(void*) ipl->imageData,ipl->height*ipl->width*ipl->nChannels*d);//needs tests
    c += ipl->height*ipl->width*d*ipl->nChannels;
  }

  cvReleaseImage(&(img->ipl));
  vglSetContext(img, VGL_RAM_CONTEXT);
  //vglUpload(img); //must be fixed before enabling

  return img;
}

/** Print information about image.

    Print width, height, depth and number of channels

 */
void iplPrintImageInfo(IplImage* ipl, char* msg){
        if (msg){
            printf("====== %s:\n", msg);
	}
	else
	{
            printf("====== iplPrintImageInfo:\n");
	}
        printf("Image @ %p: w x h = %d(%d) x %d\n", 
                ipl, ipl->width, ipl->widthStep, ipl->height);
        printf("nChannels = %d\n", ipl->nChannels);
        printf("depth = ");
        switch (ipl->depth){
          case IPL_DEPTH_1U:  printf("IPL_DEPTH_1U");  break; 
          case IPL_DEPTH_8U:  printf("IPL_DEPTH_8U");  break; 
          case IPL_DEPTH_16U: printf("IPL_DEPTH_16U"); break; 
          case IPL_DEPTH_32F: printf("IPL_DEPTH_32F"); break; 
          case IPL_DEPTH_8S:  printf("IPL_DEPTH_8S");  break; 
          case IPL_DEPTH_16S: printf("IPL_DEPTH_16S"); break; 
          case IPL_DEPTH_32S: printf("IPL_DEPTH_32S"); break; 
          default: printf("unknown");
	}
        printf("\n");
}

/** Print information about image.

    Print width, height, depth, number of channels, OpenGL texture handler, 
    OpenGL FBO handler, and current valid context (RAM, GPU or FBO).

 */
void vglPrintImageInfo(VglImage* image, char* msg){
    //IplImage* ipl = image->ipl;
    if (msg){
        printf("====== %s:\n", msg);
    }
    else
    {
        printf("====== vglPrintImageInfo:\n");
    }
    printf("Image @ %p: w x h x l = %d x %d x %d\n", 
    image, image->shape[VGL_WIDTH], image->shape[VGL_HEIGHT], image->shape[VGL_LENGTH]);
    printf("Ipl @ %p\n", image->ipl);
    printf("ndarray @ %p\n", image->ndarray); 
    printf("ndim = %d\n", image->ndim);
    printf("nChannels = %d\n", image->nChannels);
    printf("depth = ");
    switch (image->depth){
        case IPL_DEPTH_1U:  printf("IPL_DEPTH_1U");  break; 
        case IPL_DEPTH_8U:  printf("IPL_DEPTH_8U");  break; 
        case IPL_DEPTH_16U: printf("IPL_DEPTH_16U"); break; 
        case IPL_DEPTH_32F: printf("IPL_DEPTH_32F"); break; 
        case IPL_DEPTH_8S:  printf("IPL_DEPTH_8S");  break; 
        case IPL_DEPTH_16S: printf("IPL_DEPTH_16S"); break; 
        case IPL_DEPTH_32S: printf("IPL_DEPTH_32S"); break; 
        default: printf("unknown");
    }
    printf("\n");
    printf("TEX = %d\n", image->tex);
    printf("FBO = %d\n", image->fbo);
#ifdef __OPENCL__
    printf("OCL @ %p\n", image->oclPtr);
#endif
    printf("Context = %d\n", image->inContext);
}

/** Print image pixels in text format to stdout

 */
void vglPrintImageData(VglImage* image, char* msg /*= NULL*/, char* format /*= "%c"*/){
    if (msg){
        printf("====== %s:\n", msg);
    }
    else
    {
        printf("====== vglPrintImageData:\n");
    }
    int w = image->shape[0] * image->nChannels * image->getBytesPerPixel();
    int h = image->shape[1];
    int ndarraySize = image->getTotalSizeInBytes();
    char* ptr = image->getImageData();

    for(int i = 0; i < ndarraySize;)
    {
        if (i % w == 0)
	{
            printf("%d: ", i / w);
	}
        printf(format, ((unsigned char*)ptr)[i]);
        i++;
        if (i % w == 0)
	{
            printf("\n"); 
        }
        else if (i % 8 == 0)
	{
            printf(" "); 
        }
    }
}


/** Copy data from src texture to dst texture
 */
void vglCopyImageTex(VglImage* src, VglImage* dst)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	vglCheckContext(src, VGL_GL_CONTEXT);

	glBindTexture(GL_TEXTURE_2D, src->tex);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
	ERRCHECK()

	glViewport(0, 0, 2*dst->shape[VGL_WIDTH], 2*dst->shape[VGL_HEIGHT]);

	glBegin(GL_QUADS);
		glTexCoord2f( 0.0,  0.0);
		glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

		glTexCoord2f( 1.0,  0.0);
		glVertex3f (  0.0, -1.0, 0.0); //Right Up

		glTexCoord2f( 1.0,  1.0);
		glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

		glTexCoord2f( 0.0,  1.0);
		glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
	glEnd();

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	vglSetContext(dst, VGL_GL_CONTEXT);
}


/*
void vglAxis(void){

  glLineWidth(3.0);

  glColor4f(0.1, 0.1, 1.0, 0.1);
  glBegin(GL_LINES);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f( 0.0,  0.0, 99.0);
  glEnd();

  glColor4f(1.0, 0.1, 0.1, 0.1);
  glBegin(GL_LINES);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f(99.0,  0.0,  0.0);
  glEnd();

  glColor4f(0.1, 1.0, 0.1, 0.1);
  glBegin(GL_LINES);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f( 0.0, 99.0,  0.0);
  glEnd();

  glColor4f(1.0, 1.0, 1.0, 0.0);
}
*/

/** Copy data from src texture to dst texture using a fragment shader
 */
void vglCopyImageTexFS(VglImage* src, VglImage* dst)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()

  static GLuint f = 0;
  if (f == 0){
    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    //f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/shader_15_1.frag");
    f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/vglCopy.frag");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

  glUseProgram(f);

  glUniform2f(glGetUniformLocation(f, (char*)"tex_size"), src->ipl->width, src->ipl->height);

  glUniform1f(glGetUniformLocation(f, (char*)"width"), src->ipl->width);
  glUniform1f(glGetUniformLocation(f, (char*)"height"), src->ipl->height);

  ERRCHECK()

    //printf("Using fragment shader = %d.\n", f);

	glViewport(0, 0, 2*dst->shape[VGL_WIDTH], 2*dst->shape[VGL_HEIGHT]);

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Copy data from src texture to dst texture using a 
    fragment shader and a vertex shader
 */
void vglCopyImageTexVFS(VglImage* src, VglImage* dst)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()


  glUseProgram(0);

  static GLuint p = 0;
  if (p == 0){
    fprintf(stdout, "VERTEX SHADER\n====================\n");
    p = vglShaderLoad(GL_VERTEX_SHADER, (char*)"VS/vglPassThrough.vert");
    if (!p){
      fprintf(stderr, "%s: %s: Error loading vertex shader.\n", __FILE__, __FUNCTION__);
      exit(1);
    }
    ERRCHECK()

    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    p = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/vglNot.frag");
    if (!p){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
    }
    ERRCHECK()
  }

  glUseProgram(p);

  /*
  int x = glGetUniformLocation(p, "tex_size");
  printf("uniform location of tex_size = %d\n", x);
  ERRCHECK()

  glUniform2f(glGetUniformLocation(p, "tex_size"), src->ipl->width, src->ipl->height);
  ERRCHECK()

  glUniform1f(glGetUniformLocation(p, "width"), src->ipl->width);
  ERRCHECK()

  glUniform1f(glGetUniformLocation(p, "height"), src->ipl->height);
  ERRCHECK()
  */
      int retval;
      glGetIntegerv(GL_CURRENT_PROGRAM, &retval);
      //printf("current program = %d\n", retval);

  glViewport(0, 0, 2*dst->shape[VGL_WIDTH], 2*dst->shape[VGL_HEIGHT]);

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Flip image vertically, that is, top becomes bottom.

    Image flip done by texture mapping, that is, by the fixed pipeline.
 */
void vglVerticalFlip2(VglImage* src, VglImage* dst){
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()

  glViewport(0, 0, 2*dst->shape[VGL_WIDTH], 2*dst->shape[VGL_HEIGHT]);

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/*
void vglGreen(VglImage* src, VglImage* dst){

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();

  vglSetContext(dst, VGL_GL_CONTEXT);
}
*/

/** Flip image horizontally, that is, left becomes right.

    Image flip done by texture mapping, that is, by the fixed pipeline.
 */
void vglHorizontalFlip2(VglImage* src, VglImage* dst){
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

      glBindTexture(GL_TEXTURE_2D, src->tex);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
      ERRCHECK()

  glViewport(0, 0, 2*dst->shape[VGL_WIDTH], 2*dst->shape[VGL_HEIGHT]);

      glBegin(GL_QUADS);
          glTexCoord2f( 1.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 0.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 0.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 1.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  vglSetContext(dst, VGL_GL_CONTEXT);
}

void vglClear(VglImage* image, float r, float g, float b, float a){
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);
      ERRCHECK()

      glClearColor(r, g, b, a);
      glClear(GL_COLOR_BUFFER_BIT);

  vglSetContext(image, VGL_GL_CONTEXT);
}


/** Morpological opening by square structuring element. Opening is an 
    erosion followed by a dilation. A buffer is required. Source 
    and destination may be the same.

    The structuring element is a 3x3 square. The parameter 
    "times" indicates how many times the erosion will be applied.

*/
void vglOpenSq3(VglImage* src, VglImage* dst, VglImage* buf, int times){
  int i;
  int in_buf, in_dst;

  vglCheckContext(src, VGL_GL_CONTEXT);

  vglErodeSq3(src, buf);
  in_buf = 1;
  for (i = 1; i < times; i++){
    if (in_buf){
      vglErodeSq3(buf, dst);
      in_dst = 1; in_buf = 0;
    }
    else{
      vglErodeSq3(dst, buf);
      in_dst = 0; in_buf = 1;
    }
  }
  for (i = 0; i < times; i++){
    if (in_buf){
      vglDilateSq3(buf, dst);
      in_dst = 1; in_buf = 0;
    }
    else{
      vglDilateSq3(dst, buf);
      in_dst = 0; in_buf = 1;
    }
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Morphological closing by square structuring element. A buffer 
    is required. Source and destination may be the same.

    The structuring element is a 3x3 square. The parameter 
    "times" indicates how many times the closing will be applied.

*/
void vglCloseSq3(VglImage* src, VglImage* dst, VglImage* buf, int times){
  int i;
  int in_buf, in_dst = 0;

  vglCheckContext(src, VGL_GL_CONTEXT);

  vglDilateSq3(src, buf);
  in_buf = 1;
  for (i = 1; i < times; i++){
    if (in_buf){
      vglDilateSq3(buf, dst);
      in_dst = 1; in_buf = 0;
    }
    else{
      vglDilateSq3(dst, buf);
      in_dst = 0; in_buf = 1;
    }
  }
  for (i = 0; i < times; i++){
    if (in_buf){
      vglErodeSq3(buf, dst);
      in_dst = 1; in_buf = 0;
    }
    else{
      vglErodeSq3(dst, buf);
      in_dst = 0; in_buf = 1;
    }
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Morpological erosion by square structuring element 3x3. A buffer is 
    required. Source and destination may be the same.

    The structuring element is a 3x3 square. The parameter 
    "times" indicates how many times the erosion will be applied.

*/
void vglErodeSq3Sep(VglImage* src, VglImage* dst, VglImage* buf, int times){
  int i;
  int in_buf, in_dst;

  vglCheckContext(src, VGL_GL_CONTEXT);

  vglErodeHL3(src, buf);
  vglErodeVL3(buf, dst);
  for (i = 1; i < times; i++){
    vglErodeHL3(dst, buf);
    vglErodeVL3(buf, dst);
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Morpological erosion by square structuring element 5x5. A buffer is 
    required. Source and destination may be the same.

    The structuring element is a 5x5 square. The parameter 
    "times" indicates how many times the erosion will be applied.

*/
void vglErodeSq5Sep(VglImage* src, VglImage* dst, VglImage* buf, int times){
  int i;
  int in_buf, in_dst;

  vglCheckContext(src, VGL_GL_CONTEXT);

  vglErodeHL5(src, buf);
  vglErodeVL5(buf, dst);
  for (i = 1; i < times; i++){
    vglErodeHL5(dst, buf);
    vglErodeVL5(buf, dst);
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}

/** Morpological conditional erosion by cross structuring 
    element 3x3. A buffer is required. Source and destination 
    may be the same.

    The structuring element is a 3x3 cross. The parameter 
    "times" indicates how many times the erosion will be applied.

*/
void vglCErodeCross3(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buf, int times){
  int i;

  vglCheckContext(src, VGL_GL_CONTEXT);
  vglCheckContext(mask, VGL_GL_CONTEXT);

  vglOr(src, mask, dst);
  for (i = 0; i < times; i++){
    vglErodeCross3(dst, buf);
    vglOr(buf, mask, dst);
  }

  vglSetContext(dst, VGL_GL_CONTEXT);
}


/** Save image data to PPM file, 3 channels, unsigned byte

    Time to save a VGA image = 3.5ms
*/
int SavePPM(char* filename, int w, int h, void* savebuf){
    FILE *fp = fopen(filename, "wb");
    fprintf(fp, "P6\n%d %d\n255\n", w, h);
    fwrite(savebuf, w * h * 3, 1, fp);
    fclose(fp);
    return 0;
}


/** Save image to PPM file, 3 channels, unsigned byte

*/
int vglSavePPM(VglImage* img, char* filename){
    vglCheckContext(img, VGL_GL_CONTEXT);
    vglDownloadPPM(img);
    return SavePPM(filename, img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], img->ipl->imageData);
}

/** Save image data to PGM file, 1 channel, unsigned byte

*/
int SavePGM(char* filename, int w, int h, void* savebuf){
    FILE *fp = fopen(filename, "wb");
    fprintf(fp, "P5\n%d %d\n255\n", w, h);
    fwrite(savebuf, w * h, 1, fp);
    fclose(fp);
    return 0;
}

/** Save image to PGM file, 1 channel, unsigned byte

*/
int vglSavePGM(VglImage* img, char* filename){
    vglCheckContext(img, VGL_GL_CONTEXT);
    vglDownloadPGM(img);
    return SavePGM(filename, img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], img->ipl->imageData);
}

/** Load image data from PGM file, 1 channel, unsigned byte

*/
IplImage* LoadPGM(char* filename){
    FILE *fp = fopen(filename, "r");
    if (!fp){
      fprintf(stderr, "%s: %s: Error loading PGM file %s\n", __FILE__, __FUNCTION__, filename);
      return NULL;
    }
    char header[15];
    fgets(header, 3, fp);
    if (header[0] != 'P' || header[1] != '5' || header[2] != '\0'){
      fprintf(stderr, "LoadPGM: file %s is not in pgm-format\n", filename);
    }
    else{
      printf("P5 OK!!!\n");
    }

    //fprintf(fp, "P5\n%d %d\n255\n", w, h);
    //fwrite(savebuf, w * h, 1, fp);
    fclose(fp);

    //TODO finish implementation.
    return 0;
}

/** Load image from PGM file, 1 channel, unsigned byte

*/
VglImage* vglLoadPGM(char* filename){
    IplImage* ipl = LoadPGM(filename);
    VglImage* vgl = vglCreateImage(ipl);
    cvReleaseImage(&ipl);
    return vgl;
}

/** Save compressed YUV411 image data to file.

    Requires one half of the disk space required to save an uncompressed PPM.
*/
int SaveYUV411(char* filename, int w, int h, void* savebuf){
    w = (int) 1.5 * w;
    FILE *fp = fopen(filename, "wb");
    fprintf(fp, "P5\n%d %d\n255\n", w, h);
    fwrite(savebuf, w * h, 1, fp);
    fclose(fp);
    return 0;
}

/** Distance transform given by elementary cross.

    Perform successive erosions on input image thresholded to 1/256. The sum of 
the erosions results is returned as the distance transform result.

  */
void vglDistTransformCross3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times /*=1*/){
    vglThresh(src, buf, 0.0, 1.0/256.0);
    vglClear(dst, 0.0, 0.0, 0.0);
    for(int i = 0; i < times; i++){
      if (i % 2 == 0){
        vglErodeCross3(buf, buf2);
        vglSum(buf2, dst, dst);
      }
      else{
        vglErodeCross3(buf2, buf);
        vglSum(buf, dst, dst);
      }
    }
}

/** Distance transform given by square 3x3.

    Perform successive erorions on input image thresholded to 1/256. The sum of 
the erosions results is returned as the distance transform result.

  */
void vglDistTransformSq3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times /*=1*/){
    vglThresh(src, buf, 0.0, 1.0/256.0);
    vglClear(dst, 0.0, 0.0, 0.0);
    for(int i = 0; i < times; i++){
      if (i % 2 == 0){
        vglErodeSq3(buf, buf2);
        vglSum(buf2, dst, dst);
      }
      else{
        vglErodeSq3(buf2, buf);
        vglSum(buf, dst, dst);
      }
    }
}

/** Distance transform given by alternating an elementary cross and a square 3x3.

    Perform successive erorions on input image thresholded to 1/256. The sum of 
the erosions results is returned as the distance transform result.

  */
void vglDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times /*=1*/){
    vglThresh(src, buf, 0.0, 1.0/256.0);
    vglClear(dst, 0.0, 0.0, 0.0);
    vglCopy(buf, dst);
    for(int i = 0; i < times; i++){
      if (i % 2 == 0){
        vglErodeSq3(buf, buf2);
        vglSum(buf2, dst, dst);
      }
      else{
        vglErodeCross3(buf2, buf);
        vglSum(buf, dst, dst);
      }
    }
}

/** Get level curve of distance transform5

    Perform successive erorions on input image thresholded to 1/256. 
The returned image is the difference between the results obtained in 
the iterations "times" and "times" - 1.

  */
void vglGetLevelDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times /*=1*/){
    //vglClear(dst, 0.0, 0.0, 0.0);
    vglCopy(src, buf);
    int i;
    for(i = 0; i <= times; i++){
      if (i % 2 == 0){
        vglErodeSq3(buf, buf2);
      }
      else{
        vglErodeCross3(buf2, buf);
      }
    }
    vglAbsDiff(buf, buf2, dst);
}

/** Structuring element thinning. Algorithm by Bernard and Manzanera 1999.

    Receive as input the image to be thinned. The second image is an auxiliary 
image. The third image stores the result. Both the second and third images must
have the same size and type as the first input image.

    The fourth parameter is the number of iterations.

    Reference:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    T. M. Bernard and A. Manzanera, Improved low complexity fully parallel
        thinning algorithms, 1999

  */
void vglThinBernard(VglImage* src, VglImage* dst, VglImage* buf, int times /*=1*/){
    for(int i = 0; i < times; i++){
      vglErodeCross3(src, buf);
      vglThinBernardAux(src, buf, dst);
      vglCopy(dst, src);
    }
}

/** Structuring element thinning. Algorithm by Chin, Wan
Stover and Iverson, 1987.

    Receive as input the image to be thinned, buffer image and number 
of times to iterate.

    Neighborhood pixels are indexed as follows:

\f$
\begin{array}{ccc}

    P3  &  P2  &  P1    \\
    P4  &  P8  &  P0    \\
    P5  &  P6  &  P7

\end{array}
\f$

    Reference:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    R. T. Chin et al., A one-pass thinning algorithm and its parallel 
        implementation, 1987
  */
void vglThinChin(VglImage* src, VglImage* dst, VglImage* buf, int times /*=1*/){
    vglCopy(src, buf);
    for(int i = 0; i < times; i++){
      vglThinChinAux(buf, dst);
      vglCopy(dst, buf);
    }
}

/** Calculates baricenter of vga image.

    The resuld is stored in image RGB with one pixel.
    R = M(0, 0) = sum(     f(x, y) )
    G = M(1, 0) = sum( x * f(x, y) )
    B = M(0, 1) = sum( y * f(x, y) )

    Reference:

    William K. Pratt, Digital Image Processing, Second Edition

  */
void vglBaricenterVga(VglImage* src, double* x_avg /*= NULL*/, double* y_avg /*= NULL*/, double* pix_count /*= NULL*/){
  static VglImage* img[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  int width  = 640;
  int height = 480;

  if(src->shape[VGL_WIDTH] != 640 || src->shape[VGL_HEIGHT] != 480){
      fprintf(stderr, "%s: %s: Error: image must be 640x480.\n", __FILE__, __FUNCTION__);
  }

  if(img[1] == NULL){
    img[1]  = vglCreateImage(cvSize(width      , height      ), IPL_DEPTH_32F);
    img[2]  = vglCreateImage(cvSize(width / 5  , height      ), IPL_DEPTH_32F);
    img[3]  = vglCreateImage(cvSize(width / 20 , height      ), IPL_DEPTH_32F);
    img[4]  = vglCreateImage(cvSize(width / 20 , height / 5  ), IPL_DEPTH_32F);
    img[5]  = vglCreateImage(cvSize(width / 20 , height / 15 ), IPL_DEPTH_32F);
    img[6]  = vglCreateImage(cvSize(width / 40 , height / 30 ), IPL_DEPTH_32F);
    img[7]  = vglCreateImage(cvSize(width / 80 , height / 60 ), IPL_DEPTH_32F);
    img[8]  = vglCreateImage(cvSize(width / 160, height / 120), IPL_DEPTH_32F);
    img[9]  = vglCreateImage(cvSize(width / 320, height / 240), IPL_DEPTH_32F);
    img[10] = vglCreateImage(cvSize(width / 640, height / 480), IPL_DEPTH_32F);
  }

  vglBaricenterInit(src, img[1]);
  vglSelfSum5h(img[1], img[2]);
  vglSelfSum4h(img[2], img[3]);
  vglSelfSum5v(img[3], img[4]);
  vglSelfSum3v(img[4], img[5]);
  vglSelfSum22(img[5], img[6]);
  vglSelfSum22(img[6], img[7]);
  vglSelfSum22(img[7], img[8]);
  vglSelfSum22(img[8], img[9]);
  vglSelfSum22(img[9], img[10]);
  vglDownload(img[10]);

  float* data = (float*) img[10]->ipl->imageData;
  printf("MOMENTS = (%f, %f, %f)\n", data[0], data[1], data[2]);
  printf("(x, y)  = (%f, %f)\n", data[1]/data[2], data[0]/data[2]);
  if (x_avg != NULL){
    *x_avg = data[1]/data[2];
  }
  if (y_avg != NULL){
    *y_avg = data[0]/data[2];
  }
  if (pix_count != NULL){
    *pix_count = data[2];
  }

}

/** Test and model for functions with multiple output images.

    First parameter is input. Second and third parameters are output.

  */
void vglMultiOutput_model(VglImage*  src, VglImage*  dst, VglImage*  dst1){

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src, VGL_GL_CONTEXT);

  static GLuint f = 0;
  if (f == 0){
    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/vglMultiOutput.frag");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

  glUseProgram(f);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, src->tex);
  glUniform1i(glGetUniformLocation(f, "sampler0"),  0);
  ERRCHECK()

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
  CHECK_FRAMEBUFFER_STATUS()
  ERRCHECK()

    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, dst->tex, 0);
    //CHECK_FRAMEBUFFER_STATUS()
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, dst1->tex, 0);
  CHECK_FRAMEBUFFER_STATUS()

  glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);

  GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
  glDrawBuffers(2, buffers);
  ERRCHECK()

    //glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
    ERRCHECK()

  glViewport(0, 0, 2*dst->shape[VGL_WIDTH], 2*dst->shape[VGL_HEIGHT]);

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  glPopAttrib();

  glActiveTexture(GL_TEXTURE0);

  vglSetContext(dst, VGL_GL_CONTEXT);
  vglSetContext(dst1, VGL_GL_CONTEXT);
}

/** Test and model for IN_OUT semantics.

    First parameter is input and output. Second parameter is output.

  */
void vglInOut_model(VglImage*  dst, VglImage*  dst1){

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(dst, VGL_GL_CONTEXT);

  static GLuint f = 0;
  if (f == 0){
    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/vglInOut.frag");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

  glUseProgram(f);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, dst->tex);
  glUniform1i(glGetUniformLocation(f, "sampler0"),  0);
  ERRCHECK()

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
  CHECK_FRAMEBUFFER_STATUS()
  ERRCHECK()

    //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, dst->tex, 0);
    //CHECK_FRAMEBUFFER_STATUS()
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, dst1->tex, 0);
  CHECK_FRAMEBUFFER_STATUS()

  glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);

  GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
  glDrawBuffers(2, buffers);
  ERRCHECK()

    //glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
    ERRCHECK()

  glViewport(0, 0, 2*dst->shape[VGL_WIDTH], 2*dst->shape[VGL_HEIGHT]);

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  glPopAttrib();

  glActiveTexture(GL_TEXTURE0);

  vglSetContext(dst, VGL_GL_CONTEXT);
  vglSetContext(dst1, VGL_GL_CONTEXT);
}


/** Test and model for functions with multiple input images.

    First and second parameters are input. Third parameter is output.

  */
void vglMultiInput_model(VglImage*  src0, VglImage*  src1, VglImage*  dst){

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  vglCheckContext(src0, VGL_GL_CONTEXT);
  vglCheckContext(src1, VGL_GL_CONTEXT);

  static GLuint f = 0;
  if (f == 0){
    fprintf(stdout, "VERTEX SHADER\n====================\n");
    f = vglShaderLoad(GL_VERTEX_SHADER, (char*)"VS/vglPassThrough.vert");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading vertex shader.\n", __FILE__, __FUNCTION__);
      exit(1);
    }
    ERRCHECK()

    fprintf(stdout, "FRAGMENT SHADER\n====================\n");
    f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*)"FS/vglMultiInput_model.frag");
    if (!f){
      fprintf(stderr, "%s: %s: Error loading fragment shader.\n", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

    // funciona sem shader 
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, src0->tex);

  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, src1->tex);
  // ate aqui

  glUseProgram(f);

  glActiveTexture(GL_TEXTURE1);
  //glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, src1->tex);

  glActiveTexture(GL_TEXTURE0);
  //glEnable(GL_TEXTURE_2D);
  //glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, src0->tex);

  glUniform1i(glGetUniformLocation(f, "sampler1"),  1);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, dst->fbo);
  ERRCHECK()


  ERRCHECK()

  glViewport(0, 0, 2*dst->shape[VGL_WIDTH], 2*dst->shape[VGL_HEIGHT]);

      glBegin(GL_QUADS);
          glTexCoord2f( 0.3,  0.3);
          //glMultiTexCoord2f(GL_TEXTURE1, 0.0,  0.0);
          //glMultiTexCoord2f(GL_TEXTURE0, -0.2,  -0.2);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.3,  0.3);
          //glMultiTexCoord2f(GL_TEXTURE1, 1.0,  0.0);
          //glMultiTexCoord2f(GL_TEXTURE0, 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.3,  1.3);
          //glMultiTexCoord2f(GL_TEXTURE1, 1.0,  1.0);
          //glMultiTexCoord2f(GL_TEXTURE0, 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.3,  1.3);
          //glMultiTexCoord2f(GL_TEXTURE1, 0.0,  1.0);
          //glMultiTexCoord2f(GL_TEXTURE0, 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();
  glUseProgram(0);

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);

  vglSetContext(dst, VGL_GL_CONTEXT);
}


