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
int vglGdcmSaveDicom(char* outFilename, VglImage* imagevgl, int compress);
int vglGdcmSaveDicomUncompressed(char* outFilename, VglImage* imagevgl);
int vglGdcmSaveDicomCompressed(char* outFilename, VglImage* imagevgl);
int vglGdcmSave4dDicom(char* filename, VglImage* image, int lStart, int lEnd, int compress = 0);
int convertDepthGdcmToVgl(int dcmDepth);
int convertDepthVglToGdcm(int vglDepth);

#endif

#endif


