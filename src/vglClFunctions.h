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

void vglCl3dErode(VglImage* input, VglImage* output, VglImage* buff, float* mask, int strel_size_x, int strel_size_y, int strel_size_z, int times);
void vglClErode(VglImage* input, VglImage* output, VglImage* buff, float* mask, int strel_size_x, int strel_size_y, int times);

void vglCl3dDilate(VglImage* input, VglImage* output, VglImage* buff, float* mask, int strel_size_x, int strel_size_y,int strel_size_z, int times);
void vglClDilate(VglImage* input, VglImage* output, VglImage* buff, float* mask, int strel_size_x, int strel_size_y, int times);

void vglCl3dDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times);
void vglClDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times);

void vglCl3dTopHat(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int times);
void vglClTopHat(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, float* strel, int strel_size_x, int strel_size_y, int times);


bool vglClEqual(VglImage* input1, VglImage* input2);
bool vglCl3dEqual(VglImage* input1, VglImage* input2);
bool vglClBinEqual(VglImage* input1, VglImage* input2);
bool vglCl3dBinEqual(VglImage* input1, VglImage* input2);
bool vglClNdBinEqual(VglImage* input1, VglImage* input2);
bool vglClEqual(VglImage* input1, VglImage* input2, const char* kernel_name);



/* ConditionalDilate for 3d and 2d */
void vglCl3dConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int strel_size_z);
void vglClConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, float* strel, int strel_size_x, int strel_size_y);

/* ConditionalErode for 3d and 2d */
void vglCl3dConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int strel_size_z);
void vglClConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, float* strel, int strel_size_x, int strel_size_y);

/* ConditionalDilate for 3d and 2d */
void vglCl3dConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int times);
void vglClConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int times);

/* ConditionalErode for 3d and 2d */
void vglCl3dConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int times);
void vglClConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int times);

/* ReconstructionByDilation for 3d and 2d */
void vglCl3dReconstructionByDilation(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int strel_size_z);
void vglClReconstructionByDilation(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y);

/* ReconstructionByErosion for 3d and 2d*/
void vglCl3dReconstructionByErosion(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int strel_size_z);
void vglClReconstructionByErosion(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y);

/* ReconstructionByOpening for 3d and 2d */
void vglCl3dReconstructionByOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, float* strel, int strel_size_x, int strel_size_y, int strel_size_z);
void vglClReconstructionByOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, float* strel, int strel_size_x, int strel_size_y);

/* ReconstructionByClosing for 3d and 2d */
void vglCl3dReconstructionByClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, float* strel, int strel_size_x, int strel_size_y, int strel_size_z);
void vglClReconstructionByClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, float* strel, int strel_size_x, int strel_size_y);

#define VGL_CL_FUZZY_STANDARD 1
#define VGL_CL_FUZZY_ALGEBRAIC 2
#define VGL_CL_FUZZY_BOUNDED 3
#define VGL_CL_FUZZY_DRASTIC 4
#define VGL_CL_FUZZY_DAP 5
#define VGL_CL_FUZZY_HAMACHER 6
#define VGL_CL_FUZZY_GEOMETRIC 7
#define VGL_CL_FUZZY_ARITHMETIC 8

#define VGL_CL_FUZZY_USE_GAMMA(x) (x == VGL_CL_FUZZY_DAP || x == VGL_CL_FUZZY_HAMACHER)

void vglClFuzzyErode(VglImage* src, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int type = 1, float gamma = 0.2);
void vglClFuzzyDilate(VglImage* src, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int type = 1, float gamma = 0.2);

void vglCl3dFuzzyErode(VglImage* src, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int type = 1, float gamma = 0.2);
void vglCl3dFuzzyDilate(VglImage* src, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int type = 1, float gamma = 0.2);

// Binary functions

void vglClBinDilate(VglImage* src, VglImage* dst, VglStrEl* strel);
void vglCl3dBinDilate(VglImage* src, VglImage* dst, VglStrEl* strel);
void vglClBinErode(VglImage* src, VglImage* dst, VglStrEl* strel);
void vglCl3dBinErode(VglImage* src, VglImage* dst, VglStrEl* strel);
void vglClBinNDilate(VglImage* src, VglImage* dst, VglImage* buff, VglStrEl* strel, int times);
void vglClBinNErode(VglImage* src, VglImage* dst, VglImage* buff, VglStrEl* strel, int times);
void vglClBinConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, VglStrEl* strel);
void vglClBinConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, VglStrEl* strel);
void vglClBinOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel, int times);
void vglClBinClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel, int times);
void vglClBinNOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel, int times);
void vglClBinNClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel, int times);
void vglClXdBinCopy(VglImage* src, VglImage* dst);
bool vglClXdBinEqual(VglImage* input1, VglImage* input2);
void vglClBinReconstructionByDilation(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, VglStrEl* strel);
void vglClBinReconstructionByErosion(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, VglStrEl* strel);
void vglClBinReconstructionByOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel);
void vglClBinReconstructionByClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel);
void vglClBinNConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, VglStrEl* strel, int times);
void vglClBinNConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, VglStrEl* strel, int times);
