/*********************************************************************
***                                                                ***
***  Header file vglTiffIo.h                                       ***
***                                                                ***
*********************************************************************/

#ifndef __VGLTIFFIO_H__
#define __VGLTIFFIO_H__

#ifdef __TIFF__

#include <vglImage.h>

VglImage* vglLoadTiff(char* inFilename);
VglImage* vglLoad4dTiff(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
int vglPrintTiffInfo(char* inFilename);

#endif

#endif
