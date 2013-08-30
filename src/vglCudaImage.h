/*********************************************************************
***                                                                ***
***  Header file vglCudaImage.h                                    ***
***                                                                ***
*********************************************************************/

#include <vglImage.h>

int vglCudaToGl(VglImage* img);
int vglGlToCuda(VglImage* img);
int vglCudaAlloc(VglImage* img);
int vglCudaFree(VglImage* img);

void vglCudaInvertOnPlace(VglImage* img);
int vglCudaAllocPbo(VglImage* img);
int vglCudaFreePbo(VglImage* img);
int vglCudaMapPbo(VglImage* img);
int vglCudaUnmapPbo(VglImage* img);
void vglCudaCopy(VglImage* img, VglImage* dst);
void vglCudaCopy2(VglImage* img, VglImage* dst);
void vglCudaInvert(VglImage* img, VglImage* dst);


