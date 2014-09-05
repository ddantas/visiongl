#include "vglImage.h"

void vglClFuzzyAlgErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyAlgDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyGeoErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyGeoDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyArithErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyArithDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyBoundErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyBoundDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyDrasticErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyDrasticDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y);
void vglClFuzzyDaPErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y, float gama = 0.2f);
void vglClFuzzyDaPDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y, float gama = 0.2f);
void vglClFuzzyHamacherErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y, float gama = 0.2f);
void vglClFuzzyHamacherDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y, float gama = 0.2f);