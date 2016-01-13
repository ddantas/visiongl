#ifndef __VGLCLSHAPE_H__
#define __VGLCLSHAPE_H__


#ifdef __OPENCL_VERSION__
// OpenCL
typedef struct VglClShape{ 
  int ndim;
  int shape[11];
  int offset[11];
  int size;
} VglClShape;
#else
// API
typedef struct VglClShape{ 
  cl_int ndim;
  cl_int shape[11];
  cl_int offset[11];
  cl_int size;
} VglClShape;
#endif


#endif
