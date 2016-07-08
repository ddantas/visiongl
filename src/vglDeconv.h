/*********************************************************************
***                                                                ***
***  Header file vglDeconv.h                                       ***
***                                                                ***
***                                                                ***
***                                                                ***
*********************************************************************/

#ifndef __VGLDECONV_H__
#define __VGLDECONV_H__

#include <vglImage.h>

void vglSaveColorDeconv(char *outFilename, VglImage *imagevgl, double *mInitial, int find3rdColor = 0);
VglImage* vglColorDeconv(VglImage *imagevgl, double *mInitial, int find3rdColor = 0);

#endif
