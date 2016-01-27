
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

  int shape[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int ndim = 3;
  if (argc == 5)
  {
    shape[VGL_SHAPE_D3] = i_n - i_0 + 1;
  }
  else if ( (argc == 8) && (strcmp(argv[7], "-1d") == 0) )
  {
    ndim = 1;
    for (int i = 0; i < 2; i++)
    {
      shape[1 + i] = atoi(argv[5 + i]);
    }
  }
  else
  {
    ndim = argc - 5;
    for (int i = 0; i < ndim; i++)
    {
      shape[1 + i] = atoi(argv[5 + i]);
    }
  }

  // start: Saving original shape
  char* tmpfilename = (char*) malloc(strlen(infilename) + 256);
  sprintf(tmpfilename, infilename, i_0);
  VglImage* imgSingle = vglLoadImage((char*) tmpfilename);
  int origShape[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  origShape[VGL_SHAPE_NCHANNELS] = imgSingle->getNChannels();
  origShape[VGL_SHAPE_WIDTH] = imgSingle->getWidth();
  origShape[VGL_SHAPE_HEIGHT] = imgSingle->getHeight();
  origShape[VGL_SHAPE_LENGTH] = i_n - i_0 + 1;
  VglShape* origVglShape = new VglShape(origShape, 3);
  // end: Saving original shape

  VglImage* img = vglLoadNdImage((char*) infilename, i_0, i_n, shape, ndim);

  vglPrintImageInfo(img);

  // This call to vglClForceAsBuf makes img to be treated in OpenCL context as buffer instead of image even if it has 2 or 3 dimensions. 
  // N-dimensional data must be stored as buffer in order to use vglClNd* functions.
  // Must call vglClForceAsBuf right after img creation, and before creating out image from img in order to propagate the clForceAsBuf property.
  vglClForceAsBuf(img);
  VglImage* out = vglCreateImage(img);
  vglPrintImageInfo(img, (char*)"Input image");

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
  
  switch (option):
  {
    case 1:
      vglStrEl = new VglStrEl(VGL_STREL_MEAN, ndim);
      vglClNdConvolution(img, out, vglStrEl);
      break;
    case 2:
      vglStrEl = new VglStrEl(VGL_STREL_CUBE, ndim);
      vglClNdDilation(img, out, vglStrEl);
      break;
    case 3:
      vglStrEl = new VglStrEl(VGL_STREL_CROSS, ndim);
      vglClNdDilation(img, out, vglStrEl);
      break;
    case 4:
      vglClNdNot(img, out);
      break;
    case 5:
      char thresh = 30;
      vglClNdThreshold(img, out, thresh);
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

  vglSaveNdImage(out, (char*) "/tmp/clnd_%04d.tif", 0);
  printf("CHK500 \n");

  return 0;
}
