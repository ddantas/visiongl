
#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"
#include "cl2cpp_ND.h"
#include "glsl2cpp_shaders.h"

#include "vglShape.h"
#include "vglStrEl.h"

#include "demo/timer.h"


int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads a stack of image files and saves \n\
a copy after a 3d erosion. Usage is as follows:\n\
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

  int argD1 = 6;

  if (argc < argD1)
  {
    printf("%s", usage);
    exit(1);
  }

  char *inFilename = argv[1]; // name of the input file
  int i_0 = atoi(argv[2]);
  int i_n = atoi(argv[3]);
  int nSteps = atoi(argv[4]);
  char *outPath = argv[5]; // name of the output folder

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
  else if ( (argc == argD1 + 3) && (strcmp(argv[argD1 + 2], "-1d") == 0) )
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
  char* tmpFilename = (char*) malloc(strlen(inFilename) + 256);
  sprintf(tmpFilename, inFilename, i_0);
  VglImage* imgSingle = vglLoadImage((char*) tmpFilename);
  int origShape[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  origShape[VGL_SHAPE_NCHANNELS] = imgSingle->getNChannels();
  origShape[VGL_SHAPE_WIDTH] = imgSingle->getWidth();
  origShape[VGL_SHAPE_HEIGHT] = imgSingle->getHeight();
  origShape[VGL_SHAPE_LENGTH] = i_n - i_0 + 1;
  VglShape* origVglShape = new VglShape(origShape, 3);
  // end: Saving original shape

  VglImage* img = vglLoadNdImage((char*) inFilename, i_0, i_n, shape, ndim);
  vglPrintImageInfo(img);

  // This call to vglClForceAsBuf makes img to be treated in OpenCL context as buffer instead of image even if it has 2 or 3 dimensions. 
  // N-dimensional data must be stored as buffer in order to use vglClNd* functions.
  // Must call vglClForceAsBuf right after img creation, and before creating out image from img in order to propagate the clForceAsBuf property.
  vglClForceAsBuf(img);
  VglImage* out = vglCreateImage(img);
  vglPrintImageInfo(img, (char*)"Input image");

  VglStrEl* seCross = new VglStrEl(VGL_STREL_CROSS, ndim);
  VglStrEl* seCube = new VglStrEl(VGL_STREL_CUBE, ndim);
  VglStrEl* seMean = new VglStrEl(VGL_STREL_MEAN, ndim);
  seMean->print();

  // Benchmarks:
  int p;


  //First call to n-dimensional mean
  TimerStart();
  vglClNdConvolution(img, out, seMean);
  vglClFlush();
  printf("First call to             Convolution nD:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations Blur 3x3
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdConvolution(img, out, seMean);
  }
  vglClFlush();
  printf("Time spent on %8d    Convolution nD:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  sprintf(outFilename, "%s%s", outPath, "/clnd_conv_mean_%04d.tif");
  vglSaveNdImage(out, (char*) outFilename, i_0);


  //First call to n-dimensional dilation
  TimerStart();
  vglClNdDilate(img, out, seCube);
  vglClFlush();
  printf("First call to           Dilation nD cube:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional dilation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdConvolution(img, out, seMean);
  }
  vglClFlush();
  printf("Time spent on %8d  Dilation nD cube:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  sprintf(outFilename, "%s%s", outPath, "/clnd_dilate_cube_%04d.tif");
  vglSaveNdImage(out, (char*) outFilename, i_0);


  //Total time spent on n operations n-dimensional dilation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdConvolution(img, out, seMean);
  }
  vglClFlush();
  printf("Time spent on %8d Dilation nD cross:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  sprintf(outFilename, "%s%s", outPath, "/clnd_dilate_cross_%04d.tif");
  vglSaveNdImage(out, (char*) outFilename, i_0);


  //First call to n-dimensional negation
  TimerStart();
  vglClNdNot(img, out);
  vglClFlush();
  printf("First call to                   Negation:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional negation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdNot(img, out);
  }
  vglClFlush();
  printf("Time spent on %8d          Negation:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  sprintf(outFilename, "%s%s", outPath, "/clnd_invert_%04d.tif");
  vglSaveNdImage(out, (char*) outFilename, i_0);


  char thresh = 30;
  //First call to n-dimensional threshold
  TimerStart();
  vglClNdThreshold(img, out, thresh);
  vglClFlush();
  printf("First call to                  Threshold:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional negation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdThreshold(img, out, thresh);
  }
  vglClFlush();
  printf("Time spent on %8d         Threshold:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  sprintf(outFilename, "%s%s", outPath, "/clnd_invert_%04d.tif");
  vglSaveNdImage(out, (char*) outFilename, i_0);


  //First call to n-dimensional copy
  TimerStart();
  vglClNdCopy(img, out);
  vglClFlush();
  printf("First call to                       Copy:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional negation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdCopy(img, out);
  }
  vglClFlush();
  printf("Time spent on %8d              Copy:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  sprintf(outFilename, "%s%s", outPath, "/clnd_invert_%04d.tif");
  vglSaveNdImage(out, (char*) outFilename, i_0);


  //First call to n-dimensional Copy CPU->GPU
  vglCheckContext(img, VGL_RAM_CONTEXT);
  TimerStart();
  vglSetContext(img, VGL_RAM_CONTEXT);
  vglClUpload(img);
  vglClFlush();
  printf("First call to              Copy CPU->GPU:       %s \n", getTimeElapsedInSeconds());
  //Total time spent on n operations Copy CPU->GPU
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglSetContext(img, VGL_RAM_CONTEXT);
    vglClUpload(img);
  }
  vglClFlush();
  printf("Time spent on %8d     Copy CPU->GPU:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  sprintf(outFilename, "%s%s", outPath, "/clnd_invert_%04d.tif");
  vglSaveNdImage(out, (char*) outFilename, i_0);


  //First call to n-dimensional Copy GPU->CPU
  vglCheckContext(img, VGL_CL_CONTEXT);
  TimerStart();
  vglSetContext(img, VGL_CL_CONTEXT);
  vglClDownload(img);
  vglClFlush();
  printf("First call to              Copy GPU->CPU:       %s \n", getTimeElapsedInSeconds());
  //Total time spent on n operations Copy GPU->CPU
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglSetContext(img, VGL_CL_CONTEXT);
    vglClDownload(img);
  }
  vglClFlush();
  printf("Time spent on %8d     Copy GPU->CPU:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  sprintf(outFilename, "%s%s", outPath, "/clnd_invert_%04d.tif");
  vglSaveNdImage(out, (char*) outFilename, i_0);


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
      vglClNdConvolution(img, out, seMean);
      break;
    case 2:
      vglClNdDilate(img, out, seCube);
      break;
    case 3:
      vglClNdDilate(img, out, seCross);
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
  vglSaveNdImage(out, (char*) "/tmp/clnd_%04d.tif", 0);

  return 0;
}
