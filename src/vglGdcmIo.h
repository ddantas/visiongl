/*********************************************************************
***                                                                ***
***  Header file vglGdcmIo.h                                       ***
***                                                                ***
*********************************************************************/

#ifndef __VGLGDCMIO_H__
#define __VGLGDCMIO_H__

#ifdef __GDCM__

#include <vglImage.h>

VglImage vglGdcmLoadDicom(char* filename, char* outfilename);
int vglGdcmSaveDicom(VglImage imagevgl, char* outfilename);
int vglGdcmSaveDicomCompressed(VglImage imagevgl, char* outfilename);

#endif

#endif


