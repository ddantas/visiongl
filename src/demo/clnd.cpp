
#include "vglImage.h"
#include "vglClImage.h"
#include "cl2cpp_shaders.h"
#include "cl2cpp_ND.h"
#include "glsl2cpp_shaders.h"

#include "vglShape.h"
#include "vglStrEl.h"

// strcmp, strlen
#include <string.h>

int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads a stack of image files and saves \n\
a copy after a ND operation. Usage is as follows:\n\
\n\
demo_clnd <input file> <index 0> <index n> <output folder>\n\
\n\
where both input and output files require a printf-like integer\
format specifier (%d) which will be replaced by the integers\
from index 0 to index n\n\
\n\
    Optionally it's possible to define an alternative geometry\
to the image by adding the dimension sizes after the output\
folder as follows.\
demo_clnd <input file> <index 0> <index n> <output folder> <d1> <d2> ... <dn>\n\
\n\
    Leave d1 and d2 as zero to use instead the image dimensions\
obtained from the image file.\
\n\
";

  int argD1 = 5;

  if (argc < argD1)
  {
    printf("%s", usage);
    exit(1);
  }

  char *inFilename = argv[1]; // name of the input file
  int i_0 = atoi(argv[2]);
  int i_n = atoi(argv[3]);
  char *outPath = argv[4]; // name of the output folder

  char *outFilename = (char*) malloc(strlen(outPath));
  sprintf(outFilename, "%s/%%05d.jpg", outPath);
  printf("outFilename = %s\n", outFilename);

  //vglInit(500,500);
  vglClInit();

  int shape[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int ndim = 3;
  if (argc == argD1)
  {
    shape[VGL_SHAPE_D3] = i_n - i_0 + 1;
  }
  else if ( (argc == argD1 + 3) && (strcmp(argv[7], "-1d") == 0) )
  {
    ndim = 1;
    for (int i = 0; i < 2; i++)
    {
      shape[1 + i] = atoi(argv[argD1 + i]);
    }
  }
  else
  {
    ndim = argc - argD1;
    for (int i = 0; i < ndim; i++)
    {
      shape[1 + i] = atoi(argv[argD1 + i]);
    }
  }

  // start: Saving original shape
  char* tmpfilename = (char*) malloc(strlen(inFilename) + 256);
  sprintf(tmpfilename, inFilename, i_0);
  VglImage* imgSingle = vglLoadImage((char*) tmpfilename);
  int origShape[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  origShape[VGL_SHAPE_NCHANNELS] = imgSingle->getNChannels();
  origShape[VGL_SHAPE_WIDTH] = imgSingle->getWidth();
  origShape[VGL_SHAPE_HEIGHT] = imgSingle->getHeight();
  origShape[VGL_SHAPE_LENGTH] = i_n - i_0 + 1;
  VglShape* origVglShape = new VglShape(origShape, 3);
  // end: Saving original shape

  VglImage* img = vglLoadNdImage((char*) inFilename, i_0, i_n, shape, ndim);
  vglPrintImageInfo(img, (char*) "Input image");

  // This call to vglClForceAsBuf makes img to be treated in OpenCL context as buffer instead of image even if it has 2 or 3 dimensions. 
  // N-dimensional data must be stored as buffer in order to use vglClNd* functions.
  // Must call vglClForceAsBuf right after img creation, and before creating out image from img in order to propagate the clForceAsBuf property.
  vglClForceAsBuf(img);
  VglImage* out = vglCreateImage(img);
  vglPrintImageInfo(out, (char*)"Output image");

  VglStrEl* vglStrEl = new VglStrEl(VGL_STREL_MEAN, ndim);
  vglStrEl->print();

  /* Choose option:
    1 - N-dimensional mean filter
    2 - Dilation n-dimensional by hypercube
    3 - Dilation n-dimensional by hypercross
    4 - Negation
    5 - Threshold
    6 - Copy
  */    
  int option = 1;

  switch (option)
  {
    case 1:
      vglStrEl = new VglStrEl(VGL_STREL_MEAN, ndim);
      vglClNdConvolution(img, out, vglStrEl);
      break;
    case 2:
      vglStrEl = new VglStrEl(VGL_STREL_CUBE, ndim);
      vglClNdDilate(img, out, vglStrEl);
      break;
    case 3:
      vglStrEl = new VglStrEl(VGL_STREL_CROSS, ndim);
      vglClNdDilate(img, out, vglStrEl);
      break;
    case 4:
      vglClNdNot(img, out);
      break;
    case 5:
      {
        char thresh = 30;
        vglClNdThreshold(img, out, thresh);
      }
      break;
    case 6:
    default:
      vglClNdCopy(img, out);
      break;
  }

  vglClDownload(out);

  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }

  vglSaveNdImage((char*) "/tmp/clnd_%04d.tif", out, 0);

  return 0;
}
