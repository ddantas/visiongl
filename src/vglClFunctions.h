#include "vglImage.h"

int* vglClHistogram(VglImage* img_input);

//do not use, private
int* vglClSumPartialHistogram(cl_mem partial_hist, int size, int nchannels);
cl_mem vglClPartialHistogram(VglImage* img_input);
cl_mem vglCl3dPartialHistogram(VglImage* img_input);
int* vglClCumulativeSum(int* arr, int size);
int* vglClCumulativeSumNorm(int* arr, int size, int norm_total);
void vglClHistogramEq(VglImage* input, VglImage* output);
void vglCl3dHistogramEq(VglImage*input, VglImage* output);
void vglClGrayLevelTransform(VglImage* input, VglImage* output, int* transformation);
void vglCl3dGrayLevelTransform(VglImage* input, VglImage* output, int* transformation);
void vglCl3dErode(VglImage* input, VglImage* output, VglImage* buff, float* mask, int window_size_x, int window_size_y, int window_size_z, int times);
void vglClErode(VglImage* input, VglImage* output, VglImage* buff, float* mask, int window_size_x, int window_size_y, int times);
void vglCl3dDilate(VglImage* input, VglImage* output, VglImage* buff, float* mask, int window_size_x, int window_size_y,int window_size_z, int times);
void vglClDilate(VglImage* input, VglImage* output, VglImage* buff, float* mask, int window_size_x, int window_size_y, int times);

void vglCl3dDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times);
void vglClDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times);

void vglCl3dTopHat(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, float* window, int window_size_x, int window_size_y, int window_size_z, int times);
void vglClTopHat(VglImage* src, VglImage* dst, VglImage* buf, float* window, int window_size_x, int window_size_y, int times);

bool vglCl3dEqual(VglImage* input1, VglImage* input2);
bool vglClEqual(VglImage* input1, VglImage* input2);

/* ConditionalDilate for 3d and 2d */
void vglCl3dConditionalDilate(VglImage* src, VglImage* dst, VglImage* mask, float* window, int window_size_x, int window_size_y, int window_size_z);
void vglClConditionalDilate(VglImage* src, VglImage* dst, VglImage* mask, float* window, int window_size_x, int window_size_y);

/* ConditionalErode for 3d and 2d */
void vglCl3dConditionalErode(VglImage* src, VglImage* dst, VglImage* mask, float* window, int window_size_x, int window_size_y, int window_size_z);
void vglClConditionalErode(VglImage* src, VglImage* dst, VglImage* mask, float* window, int window_size_x, int window_size_y);

/* ConditionalDilate for 3d and 2d */
void vglCl3dConditionalDilate(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, float* window, int window_size_x, int window_size_y, int window_size_z, int times);
void vglClConditionalDilate(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, float* window, int window_size_x, int window_size_y, int times);

/* ConditionalErode for 3d and 2d */
void vglCl3dConditionalErode(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, float* window, int window_size_x, int window_size_y, int window_size_z, int times);
void vglClConditionalErode(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, float* window, int window_size_x, int window_size_y, int times);

/* DilateFromMaker */
void vglCl3dDilateFromMarker(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, VglImage* buff2, float* window, int window_size_x, int window_size_y,int window_size_z);
void vglClDilateFromMarker(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, VglImage* buff2, float* window, int window_size_x, int window_size_y);

void vglCl3dErodeFromMarker(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, VglImage* buff2, float* window, int window_size_x, int window_size_y,int window_size_z);
void vglClErodeFromMarker(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, VglImage* buff2, float* window, int window_size_x, int window_size_y);

/* Last Erosion
Does the LastErosion described by Junior Barrera and Gerald Banon on their book
src -> image to be last eroded
dst -> output image
*/
void vglCl3dLastErosion(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglImage* buff3, VglImage* buff4, VglImage* buff5, float* window, int window_size_x, int window_size_y,int window_size_z);