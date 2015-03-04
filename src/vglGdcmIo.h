/*********************************************************************
***                                                                ***
***  Header file vglGdcmIo.h                                       ***
***                                                                ***
*********************************************************************/

#ifndef __VGLGDCMIO_H__
#define __VGLGDCMIO_H__

#ifdef __GDCM__

#include <vglImage.h>

VglImage* vglGdcmLoadDicom(char* inFilename);
VglImage*  vglGdcmLoad4dDicom(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglGdcmSaveDicom(VglImage* imagevgl, char* outFilename, int compress);
int vglGdcmSaveDicomUncompressed(VglImage* imagevgl, char* outFilename);
int vglGdcmSaveDicomCompressed(VglImage* imagevgl, char* outFilename);
int vglGdcmSave4dDicom(VglImage* image, char* filename, int lStart, int lEnd, int compress = 0);
int convertDepthGdcmToVgl(int dcmDepth);
int convertDepthVglToGdcm(int vglDepth);

#endif

#endif


