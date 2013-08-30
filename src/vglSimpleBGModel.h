/*********************************************************************
***                                                                ***
***  Header file vglSimpleBGModel.h                                ***
***                                                                ***
*********************************************************************/

#include <vglImage.h>

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



