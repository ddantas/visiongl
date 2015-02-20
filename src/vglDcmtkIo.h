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
VglImage*  vglDcmtkLoad4dDicom(char* filename, int lStart, int lEnd, bool has_mipmap /*=0*/);
int vglDcmtkSaveDicom(VglImage* imagevgl, char* outFilename, int compress);
int vglDcmtkSaveDicomUncompressed(VglImage* imagevgl, char* outFilename);
int vglDcmtkSaveDicomCompressed(VglImage* imagevgl, char* outFilename);
int vglDcmtkSave4dDicom(VglImage* image, char* filename, int lStart, int lEnd, int compress = 0);
int convertDepthDcmtkToVgl(int dcmDepth);
int convertDepthVglToDcmtk(int vglDepth);


#endif

#endif
