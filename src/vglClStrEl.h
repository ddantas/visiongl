
#ifndef __VGLCLSTREL_H__
#define __VGLCLSTREL_H__

#include <vglConst.h>


// OpenCL
#ifdef __OPENCL_VERSION__
typedef struct VglClStrEl{ 
  float data[VGL_ARR_CLSTREL_SIZE];
  int ndim;
  int shape[VGL_ARR_SHAPE_SIZE];
  int offset[VGL_ARR_SHAPE_SIZE];
  int size;
} VglClStrEl;
#endif

// C++
#ifdef __OPENCL__
typedef struct VglClStrEl{ 
  cl_float data[VGL_ARR_CLSTREL_SIZE];
  cl_int ndim;
  cl_int shape[VGL_ARR_SHAPE_SIZE];
  cl_int offset[VGL_ARR_SHAPE_SIZE];
  cl_int size;
} VglClStrEl;
#endif

#endif
