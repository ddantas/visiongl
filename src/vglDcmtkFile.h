/*********************************************************************
***                                                                ***
***  Header file vglDcmtkFile.h                                    ***
***                                                                ***
***                                                                ***
***                                                                ***
*********************************************************************/

#ifndef __VGLDCMTK_H__
#define __VGLDCMTK_H__

#ifdef __DCMTK__

#include <vglImage.h>

VglImage vglLoadDicom(char* opt_ifname);
int vglSaveDicom(VglImage imagevgl, char* opt_ofname);
int vglSaveDicomCompressed(VglImage imagevgl, char* opt_ofname);

#endif

#endif
