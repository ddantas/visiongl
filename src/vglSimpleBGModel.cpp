
//printf
#include <stdio.h>
//#include <iostream>
//#include <string.h>

//opencv
//#include <highgui.h>
//#include <cv.h>

//opencv CvCamera
//#include <cvaux.h>

#include "vglSimpleBGModel.h"
#include "glsl2cpp_shaders.h"
#include "vglContext.h"

//Shaders
#include "glsl2cpp_BG.h"

//opencv IplImage
#ifdef __OPENCV__
  #undef __SSE2__
  #include <opencv2/core/types_c.h>
#else
  #include <vglOpencv.h>
#endif

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

VglSimpleBGModel::VglSimpleBGModel(VglImage* img_in, float std_thresh, int window_size, int num_training_images){
    this->width     = img_in->getWidth();
    this->height    = img_in->getHeight();
    this->nChannels = img_in->nChannels;
    this->depth     = img_in->depth;
    

    if (depth != IPL_DEPTH_8U){
      printf("VglSimpleBGModel: Error: depth must be IPL_DEPTH_8U\n");
      return;
    }
    if (num_training_images < 2){
      printf("VglSimpleBGModel: Error: num_training_images must be >= 2\n");
      return;
    }
    if (std_thresh <= 0.0){
      printf("VglSimpleBGModel: Error: std_thresh must be > 0.0\n");
      return;
    }
    if (window_size < 1){
      printf("VglSimpleBGModel: Error: window_size must be >= 1\n");
      return;
    }

    this->num_training_images = num_training_images;
    this->std_thresh          = std_thresh;
    this->window_size         = window_size;

    foreground      = vglCreateImage(img_in); // should be single channel
    foregroundClose = vglCreateImage(img_in); // should be single channel
    buf             = vglCreateImage(img_in); // should be single channel
    background32    = vglCreateImage(cvSize(width, height), IPL_DEPTH_32F, nChannels); // should be float
    variance32      = vglCreateImage(cvSize(width, height), IPL_DEPTH_32F, nChannels); // should be float

    vglClear(foreground,      0.0, 0.0, 0.0);
    vglClear(foregroundClose, 0.0, 0.0, 0.0);
    vglClear(buf,             0.0, 0.0, 0.0);
    vglClear(background32,    0.0, 0.0, 0.0);
    vglClear(variance32,      0.0, 0.0, 0.0);

    curr_training_image = 0;

    sobel = 0;
    color = 0;
    grayx = 0;
    grayy = 0;
    //TrainVglSimpleBGModel(img_in);

    vglSetContext(foreground,      VGL_GL_CONTEXT);
    vglSetContext(foregroundClose, VGL_GL_CONTEXT);
    vglSetContext(buf,             VGL_GL_CONTEXT);
    vglSetContext(background32,    VGL_GL_CONTEXT);
    vglSetContext(variance32,      VGL_GL_CONTEXT);

}

VglSimpleBGModel::~VglSimpleBGModel(){
  vglReleaseImage(&foreground);
  vglReleaseImage(&foregroundClose);
  vglReleaseImage(&background32);
  vglReleaseImage(&variance32);
  vglReleaseImage(&buf);

  if (sobel) vglReleaseImage(&sobel);
  if (color) vglReleaseImage(&color);
  if (grayx) vglReleaseImage(&grayx);
  if (grayy) vglReleaseImage(&grayy);
}

void VglSimpleBGModel::TrainSimpleBGModel(VglImage* img_in){
    curr_training_image++;
    printf("VglSimpleBGModel::TrainSimpleBGModel: Training image = %d\n", curr_training_image);
    float weight = 1.0/(float) (min(curr_training_image, num_training_images));

    vglTrainSimpleBGModel(img_in, background32, variance32, weight);
}

void VglSimpleBGModel::DetectFGSimpleBGModel(VglImage* img_in){

  vglDetectFGSimpleBGModel(img_in, background32, variance32, foreground, std_thresh);

}
/*
void VglSimpleBGModel::DetectFGSimpleBGModel2(VglImage* img_in){
  float std_thresh2 = std_thresh*std_thresh;
    for (int i = 0; i < height; i++){
      for (int j = 0; j < width; j++){
        unsigned char is_fg = 0;
        for (int k = 0; k < nChannels; k++){
          float var_threshold = 0.0;
          float sum_d2 = 0.0;
          float val = (float)GetPixel8U3CI(img_in, i, j, k);
          float avg = (float)GetPixel8U3CI(background, i, j, k);
          float var =        GetPixel32F3CI(variance32, i, j, k);
          float delta = val - avg;
          var_threshold = var * std_thresh2;
          sum_d2 = delta*delta;
          if (sum_d2 > var_threshold){
            is_fg = 1;
	  }
	}
        if (is_fg){
          GetPixel8U1CI(foreground, i, j) = 0xFF;
	}
	else{
          GetPixel8U1CI(foreground, i, j) = 0;
        }
      }
    }
}
*/

void VglSimpleBGModel::UpdatePartialSimpleBGModel(VglImage* img_in){
  float weight = 1/(float)window_size;

    vglUpdatePartialSimpleBGModel(img_in, foregroundClose, background32, variance32, weight);

}

void VglSimpleBGModel::UpdateSimpleBGModel(VglImage* img_in){
    if (curr_training_image < num_training_images){
      TrainSimpleBGModel(img_in);
    }
    else{
      DetectFGSimpleBGModel(img_in);
      //vglDilateSq3(foreground,      foregroundClose);
      vglCloseSq3(foreground,      foregroundClose, buf, 1);
      vglOpenSq3 (foregroundClose, foregroundClose, buf, 1);
      // Devido a incorporacao pelo fundo adicionada dilatacao inexistente no SimpleBgModel 
      /*vglDilateSq3(foregroundClose,      buf            ); 
      vglDilateSq3(buf            ,      foregroundClose); 
      vglDilateSq3(foregroundClose,      buf            ); 
      vglDilateSq3(buf            ,      foregroundClose); 
      vglDilateSq3(foregroundClose,      buf            ); 
      vglDilateSq3(buf            ,      foregroundClose); */
      UpdatePartialSimpleBGModel(img_in);
    }
}

/*
void VglSimpleBGModel::UpdateSimpleBGModelXGY(VglImage* img_in){
  if (!sobel) 
          sobel = cvCreateImage(cvSize(img_in->width, img_in->height), IPL_DEPTH_16S, img_in->nChannels);
  if (!color)
          color = cvCreateImage(cvSize(img_in->width, img_in->height), IPL_DEPTH_8U, img_in->nChannels);
  if (!grayx)
          grayx = cvCreateImage(cvSize(img_in->width, img_in->height), IPL_DEPTH_8U, 1);
  if (!grayy)
          grayy = cvCreateImage(cvSize(img_in->width, img_in->height), IPL_DEPTH_8U, 1);

          // sobel x
          cvSobel(img_in, sobel, 1, 0, 3);
          if (img_in->nChannels == 3){
            cvConvertScaleAbs(sobel, color);
            cvCvtColor(color, grayx, CV_RGB2GRAY);
	  }
	  else{
            cvConvertScaleAbs(sobel, grayx);
	  }
          // sobel y
          cvSobel(img_in, sobel, 0, 1, 3);
          if (img_in->nChannels == 3){
            cvConvertScaleAbs(sobel, color);
            cvCvtColor(color, grayy, CV_RGB2GRAY);
	  }
	  else{
            cvConvertScaleAbs(sobel, color);
	  }

          cvSetImageCOI(img_in, 1);
          cvCopy(grayx, img_in);

          cvSetImageCOI(img_in, 3);
          cvCopy(grayy, img_in);

          cvSetImageCOI(img_in, 0);

          UpdateSimpleBGModel(img_in);
}


void VglSimpleBGModel::UpdateSimpleBGModelRGL(VglImage* img_in){
  if (!sobel) 
          sobel = cvCreateImage(cvSize(img_in->width, img_in->height), IPL_DEPTH_16S, img_in->nChannels);
  if (!color) 
          color = cvCreateImage(cvSize(img_in->width, img_in->height), IPL_DEPTH_8U, 3);
  if (!grayx) 
          grayx = cvCreateImage(cvSize(img_in->width, img_in->height), IPL_DEPTH_8U, 1);

          printf("chk laplaca\n");
          cvLaplace(img_in, sobel, 1);
          printf("chk convert\n");
          cvConvertScaleAbs(sobel, color);
          printf("chk cvtcolor\n");
          cvCvtColor(color, grayx, CV_RGB2GRAY);

          printf("chk setcoi\n");
          cvSetImageCOI(img_in, 1);
          printf("chk copycoi\n");
          cvCopy(grayx, img_in);

          printf("chk setcoi0\n");
          cvSetImageCOI(img_in, 0);

          printf("chk update in RGL\n");
          UpdateSimpleBGModel(img_in);
}

*/

