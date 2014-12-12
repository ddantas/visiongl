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
