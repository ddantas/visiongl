/*********************************************************************
***                                                                ***
***  Header file vglShape                                          ***
***                                                                ***
*********************************************************************/

#ifndef __VGLSHAPE_H__
#define __VGLSHAPE_H__

#include <vglConst.h>

//CL
#ifdef __OPENCL__
#include <CL/cl.h>
#include <vglClShape.h>
#endif

// printf
#include <stdio.h>

////////// VglShape

#define VglShapeIndex1C2D(vs, i, j)       (i + j * vs->offset[2])
#define VglShapeIndexNC2D(vs, c, i, j)    (c + i * vs->offset[1] + j * vs->offset[2])
#define VglShapeIndex1C3D(vs, i, j, k)    (i + j * vs->offset[2] + k * vs->offset[3])
#define VglShapeIndexNC3D(vs, c, i, j, k) (c + i * vs->offset[1] + j * vs->offset[2] + k * vs->offset[3])

#define VglShapeIndexArr(vs, arr) (c + i * vs->offset[1] + j * vs->offset[2] + k * vs->offset[3])

class VglShape{
 public:
  int       ndim;                // 2 if conventional image, 3 if three-dimensional etc
  int       shape[VGL_MAX_DIM+1];  
  int       offset[VGL_MAX_DIM+1];
  int       size;
  int       bps;                 // bits per sample

  VglShape(VglShape* vglShape);
  VglShape(int* shape, int ndim, int bps = 8);
  VglShape(int w, int h);
  VglShape(int nChannels, int w, int h);
  VglShape(int nChannels, int w, int h, int d3);
  ~VglShape();
  void vglCreateShape(int* shape, int ndim, int bps = 8);
  void printArray(int* arr, int ndim);
  void printInfo();
  void print(char* msg = (char*)"");

  int getIndexFromCoord(int* coord);
  void getCoordFromIndex(int index, int* coord);

  int getSize();
  int getNpixels();
  int getNdim();
  int getBps();
  int* getShape();
  int* getOffset();

  int getNChannels();
  int getWidth();
  int getHeight();
  int getLength();

  int getWidthIn();
  int getHeightIn();
  int getNFrames();

  static int findBitsPerSample(int depth);
  static int findWidthStep(int bps, int width, int nChannels);

#ifdef __OPENCL__
  VglClShape* asVglClShape();
#endif

};

//VglShape* VglCreateShape(int* shape, int ndim);
//VglShape* VglCreateShape(int nChannels, int w, int h);
//VglShape* VglCreateShape(int nChannels, int w, int h, int d3);
//~VglShape();
//int getNdim();
//int getSize();

#endif

