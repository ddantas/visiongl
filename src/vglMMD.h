#include "vglImage.h"

void vglClFuzzyAlgErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyAlgDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyArithErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyArithDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyBoundErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyBoundDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);