/*********************************************************************
***                                                                ***
***  Header file vglStrEl                                          ***
***                                                                ***
*********************************************************************/

#ifndef __VGLSTREL_H__
#define __VGLSTREL_H__

#include <vglImage.h>
#include <vglShape.h>
#include <vglClStrEl.h>

//CL
#ifdef __OPENCL__
#include <CL/cl.h>
#endif

////////// VglStrEl

#define VGL_STREL_CUBE   1
#define VGL_STREL_CROSS  2
#define VGL_STREL_GAUSS  3

class VglStrEl{
 public:
  float*    data;
  VglShape*  vglShape;

  VglStrEl(float* data, VglShape* shape);
  VglStrEl(int type, int ndim);
  ~VglStrEl();
  void VglCreateStrEl(float* data, VglShape* shape);
  void print(char* msg = NULL);

  float* getData();

  int getSize();
  int getNpixels();
  int getNdim();
  int* getShape();
  int* getOffset();

#ifdef __OPENCL__
  VglClStrEl* asVglClStrEl();
#endif

};






#endif
