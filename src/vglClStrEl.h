
#ifndef __VGLCLSTREL_H__
#define __VGLCLSTREL_H__

#include <vglConst.h>

#ifdef __OPENCL_VERSION__
// OpenCL
typedef struct VglClStrEl{ 
  float data[VGL_MAX_CLSTREL_SIZE];
  int ndim;
  int shape[11];
  int offset[11];
  int size;
} VglClStrEl;
#else
// API
typedef struct VglClStrEl{ 
  cl_float data[VGL_MAX_CLSTREL_SIZE];
  cl_int ndim;
  cl_int shape[11];
  cl_int offset[11];
  cl_int size;
} VglClStrEl;
#endif

#endif
