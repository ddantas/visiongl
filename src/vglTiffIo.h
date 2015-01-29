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
int vglPrintTiffInfo(char* inFilename);

#endif

#endif
