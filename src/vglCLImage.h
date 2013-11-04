/*********************************************************************
***                                                                ***
***  Header file vglCLImage.h                                      ***
***                                                                ***
*********************************************************************/

#include <vglImage.h>

int vglCLToGl(VglImage* img);
int vglGlToCL(VglImage* img);
int vglCLAlloc(VglImage* img);
int vglCLFree(VglImage* img);

void vglCLInvertOnPlace(VglImage* img);
int vglCLAllocPbo(VglImage* img);
int vglCLFreePbo(VglImage* img);
int vglCLMapPbo(VglImage* img);
int vglCLUnmapPbo(VglImage* img);
void vglCLCopy(VglImage* img, VglImage* dst);
void vglCLCopy2(VglImage* img, VglImage* dst);
void vglCLInvert(VglImage* img, VglImage* dst);