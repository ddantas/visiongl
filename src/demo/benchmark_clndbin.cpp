
#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"
#include "cl2cpp_ND.h"
#include "cl2cpp_BIN.h"
#include "glsl2cpp_shaders.h"

#include "vglShape.h"
#include "vglStrEl.h"

#include "demo/timer.h"

// strcmp, strlen
#include <string.h>

int saveResult(VglImage* out, char* outString, char* outPath, char* outFolder, int i_0)
{
  char *cmd         = (char*) malloc(strlen(outPath) + 255);
  char* outFilename = (char*) malloc(strlen(outPath) + 255);

  sprintf(cmd, "mkdir -p %s/%s", outPath, outFolder);
  system(cmd);
  sprintf(outFilename, outString, outPath, outFolder);
  vglSaveNdImage((char*) outFilename, out, i_0);
}  


int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads a stack of image files and saves \n\
the results after benchmarking some operations. Usage is as follows:\n\
\n\
benchmark_clnd <input file> <index 0> <index n> <steps> <output folder>\n\
\n\
where both input and output files require a printf-like integer\
format specifier (%d) which will be replaced by the integers\
from index 0 to index n\n\
\n\
    Optionally it's possible to define an alternative geometry\
to the image by adding the dimension sizes after the output\
folder as follows.\
demo_clnd <input file> <index 0> <index n> <steps> <output folder> <d1> <d2> ... <dn>\n\
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
  char* outFolder;
  char* outString   = (char*) "%s/%s/out_%%05d.pbm";
  printf("outString = %s\n", outString);

  //vglInit(10,10);
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
  VglShape* vglShapeOrig8bit = new VglShape(origShape, 3);
  VglShape* vglShapeOrig1bit = new VglShape(origShape, 3, 1);
  // end: Saving original shape

  VglImage* vglIn = vglLoadNdImage((char*) inFilename, i_0, i_n, shape, ndim);
  VglShape* vglShape8bit = new VglShape(vglIn->vglShape);
  vglPrintImageInfo(vglIn);

  // start: Creating ROI
  int* pShape = vglIn->vglShape->shape;
  int roiP0[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int roiPf[VGL_ARR_SHAPE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  if (vglIn->vglShape->ndim == 1)
  {
    int w = vglIn->vglShape->getWidth();
    roiP0[1] = (int) (w * .05);
    roiPf[1] = (int) (w * .8);
  }
  else
  {
    for (int i = 1; i <= 10; i++)
    {
      roiP0[i] = (int) (pShape[i] * .05);
      roiPf[i] = (int) (pShape[i] * .8);
    }
  }
  // end: Creating ROI

  // This call to vglClForceAsBuf makes vglIn to be treated in OpenCL context as buffer instead of image even if it has 2 or 3 dimensions. 
  // N-dimensional data must be stored as buffer in order to use vglClNd* functions.
  // Must call vglClForceAsBuf right after vglIn creation, and before creating output images from vglIn in order to propagate the clForceAsBuf property.
  vglClForceAsBuf(vglIn);
  VglImage* vglGray   = vglCreateImage(vglIn);
  VglImage* vglThresh = vglCreateImage(vglIn->vglShape, IPL_DEPTH_1U);
  vglClForceAsBuf(vglThresh);
  VglImage* vglBin    = vglCreateImage(vglThresh);
  VglImage* vglRoi    = vglCreateImage(vglThresh);
  VglShape* vglShape1bit = new VglShape(vglThresh->vglShape);

  vglPrintImageInfo(vglIn, (char*)"Input image");
  vglPrintImageInfo(vglThresh, (char*)"vglThresh");
  vglThresh->vglShape->print((char*)"vglThresh");
  vglRoi->vglShape->print((char*)"vglRoi");

  VglStrEl* seCross = new VglStrEl(VGL_STREL_CROSS, ndim);
  VglStrEl* seCube = new VglStrEl(VGL_STREL_CUBE, ndim);
  VglStrEl* seMean = new VglStrEl(VGL_STREL_MEAN, ndim);
  VglStrEl* seCube1 = new VglStrEl(VGL_STREL_CUBE, 1);
  VglStrEl* seMean1 = new VglStrEl(VGL_STREL_MEAN, 1);
  VglStrEl* seCubeArr[ndim+1];
  VglStrEl* seMeanArr[ndim+1];
  seMean->print((char*) "seMean");
  int diam = 3;
  float diamf = (float)diam;
  float dataCube[3] = { 1.0,       1.0,       1.0     };
  float dataMean[3] = { 1.0/diamf, 1.0/diamf, 1.0/diamf };

  int strelShape[VGL_ARR_SHAPE_SIZE];
  for (int i = 0; i < VGL_ARR_SHAPE_SIZE; i++)
  {
    strelShape[i] = 1;
    printf("strelshape[%d] = %d\n", i, strelShape[i]);
  }
  for (int i = 1; i <= ndim; i++)
  {
    printf("creating separated strel: ndim = %d\n", ndim);
    strelShape[i] = diam;
    VglShape* tmpShape = new VglShape(strelShape, ndim);
    seCubeArr[i] = new VglStrEl(dataCube, tmpShape);
    seCubeArr[i]->print();
    seMeanArr[i] = new VglStrEl(dataMean, tmpShape);
    seMeanArr[i]->print();
    delete(tmpShape);
    strelShape[i] = 1;
  }
  printf("creating separated strel: done\n");

  int p;

if (vglIn->clForceAsBuf){
  // Benchmarks:

  //First call to n-dimensional copy
  TimerStart();
  vglClNdCopy(vglIn, vglGray);
  vglClFlush();
  printf("First call to                       Copy:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional copy
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdCopy(vglIn, vglGray);
  }
  vglClFlush();
  printf("Time spent on %8d              Copy:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(vglGray, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(vglGray, vglShapeOrig8bit);
  }
  outFolder = (char*) "clnd_copy";
  saveResult(vglGray, outString, outPath, outFolder, i_0);
  vglReshape(vglGray, vglShape8bit);
  vglPrintImageInfo(vglGray, (char*) "vglGray");
  vglGray->vglShape->print();


  char thresh = 108;
  //First call to n-dimensional threshold
  TimerStart();
  vglClNdBinThreshold(vglIn, vglThresh, thresh);
  vglClFlush();
  printf("First call to                  Threshold:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional threshold
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdBinThreshold(vglIn, vglThresh, thresh);
  }
  vglClFlush();
  printf("Time spent on %8d         Threshold:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(vglThresh, vglShapeOrig1bit);
  }
  outFolder = (char*) "clnd_thresh";
  saveResult(vglThresh, outString, outPath, outFolder, i_0);
  vglReshape(vglThresh, vglShape1bit);

  //First call to n-dimensional BinToGray
  TimerStart();
  vglClNdBinToGray(vglThresh, vglGray);
  vglClFlush();
  printf("First call to                  BinToGray:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional BinToGray
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdBinToGray(vglThresh, vglGray);
  }
  vglClFlush();
  printf("Time spent on %8d         BinToGray:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(vglGray, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(vglGray, vglShapeOrig8bit);
  }
  outFolder = (char*) "clnd_togray";
  saveResult(vglGray, outString, outPath, outFolder, i_0);
  vglReshape(vglGray, vglShape8bit);

  //First call to n-dimensional Not
  TimerStart();
  vglClNdBinNot(vglThresh, vglBin);
  vglClFlush();
  printf("First call to                        Not:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional Not
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdBinNot(vglThresh, vglBin);
  }
  vglClFlush();
  printf("Time spent on %8d               Not:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(vglBin, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(vglBin, vglShapeOrig1bit);
  }
  outFolder = (char*) "clnd_invert";
  saveResult(vglBin, outString, outPath, outFolder, i_0);
  vglReshape(vglBin, vglShape1bit);

  //First call to n-dimensional Roi
  TimerStart();
  vglClNdBinRoi(vglRoi, roiP0, roiPf);
  vglClFlush();
  printf("First call to                        Roi:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional Not
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdBinRoi(vglRoi, roiP0, roiPf);
  }
  vglClFlush();
  printf("Time spent on %8d               Roi:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(vglRoi, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(vglRoi, vglShapeOrig1bit);
  }
  outFolder = (char*) "clnd_roi";
  saveResult(vglRoi, outString, outPath, outFolder, i_0);
  vglReshape(vglRoi, vglShape1bit);



  /*
  //First call to n-dimensional mean
  TimerStart();
  vglClNdConvolution(img, out, seMean);
  vglClFlush();
  printf("First call to             Convolution nD:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional mean
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
  outFolder = (char*) "clnd_conv_mean";
  saveResult(out, outString, outPath, outFolder, i_0);


  //Total time spent on n operations n-dimensional sep. mean
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdConvolution(img, out2, seMeanArr[1]);
    for(int i = 2; i <= ndim; i++)
    {
      if (i % 2 == 0)
        vglClNdConvolution(out2, out, seMeanArr[i]);
      else
        vglClNdConvolution(out, out2, seMeanArr[i]);
    }
  }
  vglClFlush();
  printf("Time spent on %8d     Conv. nD sep.:       %s \n", nSteps, getTimeElapsedInSeconds());
  if (ndim % 2 == 1)
    vglClNdCopy(out2, out);

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_conv_sep";
  saveResult(out, outString, outPath, outFolder, i_0);


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
    vglClNdDilate(img, out, seCube);
  }
  vglClFlush();
  printf("Time spent on %8d  Dilation nD cube:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_dilate_cube";
  saveResult(out, outString, outPath, outFolder, i_0);


  //Total time spent on n operations n-dimensional sep. dilation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdDilate(img, out2, seCubeArr[1]);
    for(int i = 2; i <= ndim; i++)
    {
      if (i % 2 == 0)
        vglClNdDilate(out2, out, seCubeArr[i]);
      else
        vglClNdDilate(out, out2, seCubeArr[i]);
    }
  }
  vglClFlush();
  printf("Time spent on %8d  Dilation nD sep.:       %s \n", nSteps, getTimeElapsedInSeconds());
  if (ndim % 2 == 1)
    vglClNdCopy(out2, out);

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_conv_sep";
  saveResult(out, outString, outPath, outFolder, i_0);


  //Total time spent on n operations n-dimensional dilation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglClNdDilate(img, out, seCross);
  }
  vglClFlush();
  printf("Time spent on %8d Dilation nD cross:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_dilate_cross";
  saveResult(out, outString, outPath, outFolder, i_0);


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
  outFolder = (char*) "clnd_invert";
  saveResult(out, outString, outPath, outFolder, i_0);


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
  outFolder = (char*) "clnd_copy";
  saveResult(out, outString, outPath, outFolder, i_0);
  */
}
else
{
  // Benchmarks:
  /*

  //First call to n-dimensional mean
  TimerStart();
  vglCl3dConvolution(img, out, seMean->data, seMean->vglShape->getWidth(), seMean->vglShape->getHeight(), seMean->vglShape->getLength());
  vglClFlush();
  printf("First call to             Convolution 3D:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional mean
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglCl3dConvolution(img, out, seMean->data, seMean->vglShape->getWidth(), seMean->vglShape->getHeight(), seMean->vglShape->getLength());
  }
  vglClFlush();
  printf("Time spent on %8d    Convolution 3D:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_conv_mean";
  saveResult(out, outString, outPath, outFolder, i_0);


  //Total time spent on n operations n-dimensional sep. mean
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    VglStrEl* se = seMeanArr[1];
    vglCl3dConvolution(img, out2, se->data, se->vglShape->getWidth(), se->vglShape->getHeight(), se->vglShape->getLength());
    for(int i = 2; i <= ndim; i++)
    {
      se = seMeanArr[i];
      if (i % 2 == 0)
        vglCl3dConvolution(out2, out, se->data, se->vglShape->getWidth(), se->vglShape->getHeight(), se->vglShape->getLength());
      else
        vglCl3dConvolution(out, out2, se->data, se->vglShape->getWidth(), se->vglShape->getHeight(), se->vglShape->getLength());
    }
  }
  vglClFlush();
  printf("Time spent on %8d     Conv. 3D sep.:       %s \n", nSteps, getTimeElapsedInSeconds());
  if (ndim % 2 == 1)
    vglCl3dCopy(out2, out);

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_conv_sep";
  saveResult(out, outString, outPath, outFolder, i_0);


  //First call to n-dimensional dilation
  TimerStart();
  vglCl3dDilate(img, out, seCube->data, seCube->vglShape->getWidth(), seCube->vglShape->getHeight(), seCube->vglShape->getLength());
  vglClFlush();
  printf("First call to           Dilation 3D cube:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional dilation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglCl3dDilate(img, out, seCube->data, seCube->vglShape->getWidth(), seCube->vglShape->getHeight(), seCube->vglShape->getLength());
  }
  vglClFlush();
  printf("Time spent on %8d  Dilation 3D cube:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_dilate_cube";
  saveResult(out, outString, outPath, outFolder, i_0);


  //Total time spent on n operations n-dimensional sep. dilation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    VglStrEl* se = seMeanArr[1];
    vglCl3dDilate(img, out2, se->data, se->vglShape->getWidth(), se->vglShape->getHeight(), se->vglShape->getLength());
    for(int i = 2; i <= ndim; i++)
    {
      se = seMeanArr[i];
      if (i % 2 == 0)
        vglCl3dDilate(out2, out, se->data, se->vglShape->getWidth(), se->vglShape->getHeight(), se->vglShape->getLength());
      else
        vglCl3dDilate(out, out2, se->data, se->vglShape->getWidth(), se->vglShape->getHeight(), se->vglShape->getLength());
    }
  }
  vglClFlush();
  printf("Time spent on %8d  Dilation nD sep.:       %s \n", nSteps, getTimeElapsedInSeconds());
  if (ndim % 2 == 1)
    vglCl3dCopy(out2, out);

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_conv_sep";
  saveResult(out, outString, outPath, outFolder, i_0);


  //Total time spent on n operations n-dimensional dilation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglCl3dDilate(img, out, seCross->data, seCross->vglShape->getWidth(), seCross->vglShape->getHeight(), seCross->vglShape->getLength());
  }
  vglClFlush();
  printf("Time spent on %8d Dilation 3D cross:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_dilate_cross";
  saveResult(out, outString, outPath, outFolder, i_0);


  //First call to n-dimensional negation
  TimerStart();
  vglCl3dNot(img, out);
  vglClFlush();
  printf("First call to                   Negation:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional negation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglCl3dNot(img, out);
  }
  vglClFlush();
  printf("Time spent on %8d          Negation:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_invert";
  saveResult(out, outString, outPath, outFolder, i_0);


  float thresh = 30.0/255.0;
  //First call to n-dimensional threshold
  TimerStart();
  vglCl3dThreshold(img, out, thresh);
  vglClFlush();
  printf("First call to                  Threshold:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional negation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglCl3dThreshold(img, out, thresh);
  }
  vglClFlush();
  printf("Time spent on %8d         Threshold:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_thresh";
  saveResult(out, outString, outPath, outFolder, i_0);


  //First call to n-dimensional copy
  TimerStart();
  vglCl3dCopy(img, out);
  vglClFlush();
  printf("First call to                       Copy:       %s\n", getTimeElapsedInSeconds());
  //Total time spent on n operations n-dimensional negation
  p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    vglCl3dCopy(img, out);
  }
  vglClFlush();
  printf("Time spent on %8d              Copy:       %s \n", nSteps, getTimeElapsedInSeconds());

  vglCheckContext(out, VGL_RAM_CONTEXT);
  if (ndim <= 2)
  {
    vglReshape(out, origVglShape);
  }
  outFolder = (char*) "clnd_copy";
  saveResult(out, outString, outPath, outFolder, i_0);
  */
}

/*
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
  outFolder = (char*) "clnd_upload";
  saveResult(out, outString, outPath, outFolder, i_0);


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
  outFolder = (char*) "clnd_download";
  saveResult(out, outString, outPath, outFolder, i_0);
*/

  return 0;
}
