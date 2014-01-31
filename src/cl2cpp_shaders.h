
/*********************************************************************
***                                                                 ***
***  Source code generated by cl2cpp.pl                             ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/
#include "vglImage.h"
/** Convolution of src image by mask. Result is stored in dst image.

  */
void vglClBlurSq3(VglImage* img_input, VglImage* img_output);

/** Convolution of src image by mask. Result is stored in dst image.

  */
void vglClConvolution(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);

/** Direct copy from src to dst.

  */
void vglClCopy(VglImage* img_input, VglImage* img_output);

/** Negative of src image. Result is stored in dst image.

  */
void vglClInvert(VglImage* img_input, VglImage* img_output);

/** Threshold of src image by float parameter. Result is stored in dst image.

  */
void vglClThreshold(VglImage* src, VglImage* dst, float thresh);
