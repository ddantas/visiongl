#ifndef __VGLCLSHAPE_H__
#define __VGLCLSHAPE_H__

#include <vglConst.h>

// OpenCL
#ifdef __OPENCL_VERSION__
typedef struct VglClShape{ 
  int ndim;
  int shape[VGL_ARR_SHAPE_SIZE];
  int offset[VGL_ARR_SHAPE_SIZE];
  int size;
} VglClShape;
#endif

// API
#ifdef __OPENCL__
typedef struct VglClShape{ 
  cl_int ndim;
  cl_int shape[VGL_ARR_SHAPE_SIZE];
  cl_int offset[VGL_ARR_SHAPE_SIZE];
  cl_int size;
} VglClShape;
#endif

#endif
