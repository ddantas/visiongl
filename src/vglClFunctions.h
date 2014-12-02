#include "vglImage.h"

int* vglClHistogram(VglImage* img_input);

//do not use, private
int* vglClSumPartialHistogram(cl_mem partial_hist, int size, int nchannels);
cl_mem vglClPartialHistogram(VglImage* img_input);
cl_mem vglCl3dPartialHistogram(VglImage* img_input);
int* vglClCumulativeSum(int* arr, int size);
void vglClHistogramEq(VglImage* input, VglImage* output, int min);
void vglCl3dHistogramEq(VglImage* input, VglImage* output, int min);