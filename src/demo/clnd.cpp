
#include "vglImage.h"
#include "vglClImage.h"
#include "cl2cpp_shaders.h"
#include "cl2cpp_ND.h"
#include "glsl2cpp_shaders.h"

#include "vglShape.h"
#include "vglStrEl.h"


int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads a stack of image files and saves \n\
a copy after a 3d erosion. Usage is as follows:\n\
\n\
demo_clnd <input file> <output file> <index 0> <index n>\n\
\n\
where both input and output files require a printf-like integer\
format specifier (%d) which will be replaced by the integers\
from index 0 to index n\n\
\n\
";

  if (argc < 5)
  {
    printf("%s", usage);
    exit(1);
  }

  char *infilename = argv[1]; // name of the input file
  int i_0 = atoi(argv[2]);
  int i_n = atoi(argv[3]);
  char *outfolder = argv[4]; // name of the output folder

  char *outfilename = (char*) malloc(strlen(outfolder));
  sprintf(outfilename, "%s/%%05d.jpg", outfolder);
  printf("outfilename = %s\n", outfilename);

  //vglInit(500,500);
  vglClInit();

  int shape[VGL_ARR_SHAPE_SIZE] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int ndim = 3;
  if (argc == 5)
  {
    shape[VGL_SHAPE_D3] = i_n - i_0 + 1;
  }
  else
  {
    ndim = argc - 5;
    for (int i = 0; i < ndim; i ++)
    {
      shape[1 + i] = atoi(argv[5 + i]);
    }
  }
  ndim = 3;
  shape[3] = 336;
  //shape[4] = 24;
  //shape[5] = 7;

  VglShape* vglShape = new VglShape(shape, ndim);

  VglImage* img = vglLoadNdImage((char*) infilename, i_0, i_n, vglShape);

  // This call makes img to be treated as buffer instead of image even if it has 2 or 3 dimensions. 
  // N-dimensional data must be stored as buffer.
  // Must call right after img creation, and before creating out image from img in order to propagate the clForceAsBuf property.
  vglClForceAsBuf(img);
  VglImage* out = vglCreateImage(img);
  vglPrintImageInfo(img, (char*)"Input image");
  delete(vglShape);

  VglStrEl* vglStrEl = new VglStrEl(VGL_STREL_MEAN, ndim);
  vglStrEl->print();

  if (1)
  {
    //vglClNdNot(img, out);
    //vglClNdCopy(img, out);
    printf("CHK100 \n");
    vglClNdThreshold(img, out, 30);
    printf("CHK200 \n");
  }
  else
    vglClNdConvolution(img, out, vglStrEl);

  printf("CHK300 \n");
  vglClDownload(out);
  printf("CHK400 \n");

  vglSaveNdImage(out, (char*) "/tmp/clnd%04d.tif", 0);
  printf("CHK500 \n");

  return 0;
}
