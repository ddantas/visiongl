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

VglImage vglDcmtkLoadDicom(char* opt_ifname);
int vglDcmtkSaveDicom(VglImage imagevgl, char* opt_ofname);
int vglDcmtkSaveDicomCompressed(VglImage imagevgl, char* opt_ofname);

#endif

#endif
