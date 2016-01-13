/*********************************************************************
***                                                                ***
***  Header file vglConst                                          ***
***                                                                ***
*********************************************************************/

#ifndef __VGLCONST_H__
#define __VGLCONST_H__


#define VGL_WIN_X0   -1.0
#define VGL_WIN_X1    1.0
#define VGL_WIN_DX    (VGL_WIN_X1 - VGL_WIN_X0)
#define VGL_WIN_Y0   -1.0
#define VGL_WIN_Y1    1.0
#define VGL_WIN_DY    (VGL_WIN_Y1 - VGL_WIN_Y0)

#define VGL_MIN_WINDOW_SPLIT 1
#define VGL_DEFAULT_WINDOW_SPLIT 2
#define VGL_MAX_WINDOW_SPLIT 4
#define VGL_MAX_WINDOWS      (VGL_MAX_WINDOW_SPLIT * VGL_MAX_WINDOW_SPLIT)

#define VGL_MAX_DIM 10
#define VGL_WIDTH 0
#define VGL_HEIGHT 1
#define VGL_LENGTH 2
#define VGL_4D 3

#define VGL_SHAPE_D0        0
#define VGL_SHAPE_D1        1
#define VGL_SHAPE_D2        2
#define VGL_SHAPE_D3        3
#define VGL_SHAPE_D4        4

#define VGL_SHAPE_NCHANNELS VGL_SHAPE_D0
#define VGL_SHAPE_WIDTH     VGL_SHAPE_D1
#define VGL_SHAPE_HEIGHT    VGL_SHAPE_D2

//CL
//#ifdef __OPENCL__
#define VGL_MAX_CLSTREL_SIZE 200
//#endif

#endif

