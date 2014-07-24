/*********************************************************************
***                                                                ***
***  Header file vglDcmtkIo.h                                      ***
***                                                                ***
***                                                                ***
***                                                                ***
*********************************************************************/

#ifndef __VGLDCMTK_H__
#define __VGLDCMTK_H__

#ifdef __DCMTK__

#include <vglImage.h>
#include "dcmtk/dcmdata/dctk.h"

VglImage* vglDcmtkLoadDicom(char* inFilename);
int vglDcmtkSaveDicom(VglImage* imagevgl, char* outFilename, int compress);
int vglDcmtkSaveDicomUncompressed(VglImage* imagevgl, char* outFilename);
int vglDcmtkSaveDicomCompressed(VglImage* imagevgl, char* outFilename);
int convertDepthDcmtkToVgl(int dcmDepth);
int convertDepthVglToDcmtk(int vglDepth);

#endif

#endif
