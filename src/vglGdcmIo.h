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
int vglGdcmSaveDicom(VglImage* imagevgl, char* outFilename, int compress);
int vglGdcmSaveDicomUncompressed(VglImage* imagevgl, char* outFilename);
int vglGdcmSaveDicomCompressed(VglImage* imagevgl, char* outFilename);
int convertDepthGdcmToVgl(int dcmDepth);
int convertDepthVglToGdcm(int vglDepth);

#endif

#endif


