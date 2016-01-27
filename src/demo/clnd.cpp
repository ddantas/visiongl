
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
demo_cl3d <input file> <output file> <index 0> <index n>\n\
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

  vglInit(500,500);
  vglClInit();
  printf("0000\n");
  //VglImage* img = vglLoad3dImage(infilename, i_0, i_n);
  VglImage* img = vglLoadImage((char*) "../images/lena_gray.tif");
  vglClForceAsBuf(img);
  vglPrintImageInfo(img, (char*) "lena_gray");

  printf("0010\n");
  int shape5d[VGL_ARR_SHAPE_SIZE] = {1, 1, 1, 2, 24, 7, 1, 1, 1, 1, 1};
  printf("0020\n");
  VglShape* vglShape5d = new VglShape(shape5d, 5);
  printf("0030\n");
  VglImage* img5d = vglLoadNdImage((char*) "/home/ddantas/Downloads/mitosis/mitosis-5d%04d.tif", 0, 335, vglShape5d);
  printf("0040\n");
  VglImage* out5d = vglCreateImage(img5d);
  printf("0050\n");
  vglPrintImageInfo(img5d, (char*)"img 5d");
  printf("0060\n");
  delete(vglShape5d);
  printf("0070\n");

  VglStrEl* vglStrEl5d = new VglStrEl(VGL_STREL_GAUSS, 5);
  printf("0080\n");

  if (1)
    //vglClNdNot(img5d, out5d);
    //vglClNdCopy(img5d, out5d);
    vglClNdThreshold(img5d, out5d, 30);
  else
    vglClNdConvolution(img5d, out5d, vglStrEl5d);

  printf("0090\n");
  vglClDownload(out5d);
  printf("0100\n");

  vglSaveNdImage(out5d, (char*) "/tmp/clnd%04d.tif", 0);
  printf("0110\n");
  

 
  //return 0;
  //exit(0);


  /*
  img->ndim = 1;
  img->vglShape->ndim = 1;
  */
    
  int shape[VGL_ARR_SHAPE_SIZE] = {1, 512, 64, 8, 1, 1, 1, 1, 1, 1, 1};
  VglShape* imgShape;
  VglShape* origShape;

  imgShape = new VglShape(shape, 3);
  origShape = img->vglShape;
  img->vglShape = imgShape;
  img->ndim = 3;



  printf("0100\n");
  //VglImage* img = vglCreateImage(cvSize(1000, 1000), IPL_DEPTH_8U, 1, 1);
  printf("0200\n");
  vglPrintImageInfo(img, (char*) "img 001");
  printf("0300\n");
  if (img->nChannels == 3)
  {
    vglNdarray3To4Channels(img);
  }
  VglImage* out = vglCreateImage(img);

  //3x3x3 mask for convolution
  float* mask333 = (float*) malloc(sizeof(float)*27);

  for(int i = 0; i < 27; i++)
  {
    mask333[i] = 1.0f/27.0f;
  }
  //5x5x5 mask for convolution
  float* mask555 = (float*) malloc(sizeof(float)*125);
  for(int i = 0; i < 125; i++)
  {
     mask555[i] = 1.0f/125.0f;
  }

  float blur333[] = {1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
                     1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
                     1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
                     1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f,
                     1.0f/16.0f, 1.0f/8.0f,  1.0f/16.0f, 
                     1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
                     1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
                     1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
                     1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f};
		
  float strel[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      1, 1, 1, 1, 1, 1, 1, 1, 1,
                      1, 1, 1, 1, 1, 1, 1, 1, 1};

  printf("090\n");
  vglPrintImageInfo(out, (char*) "out:");

  //exit(0);

  printf("100\n");
  vglClNdNot(img, out);
  printf("200\n");
  //vglCl3dConvolution(img, out, blur333,3,3,3);
  //vglCl3dBlurSq3(img, out);
  //vglCl3dThreshold(img, out, 0.5);
  //vglCl3dDilate(img, out, strel, 1, 1, 5);
  vglPrintImageInfo(out, (char*) "out:");
  printf("210\n");
  vglPrintImageInfo(out, (char*) "out:");
  printf("220\n");
  vglPrintImageInfo(out, (char*) "out:");
  printf("230\n");


  vglClDownload(out);
  printf("300\n");

  if (out->nChannels == 4)
  {
    vglNdarray4To3Channels(out);
  }

  printf("400\n");
  //vglSave3dImage(out, outfilename, i_0, i_n);

  /*
  imgShape = new VglShape(shape, 2);
  out->vglShape = imgShape;
  out->ndim = 2;
  */

  printf("500\n");
  vglClDownload(out);
  printf("600\n");

  out->vglShape = origShape;
  vglSaveImage(out, (char*) "/tmp/teste.png");
  out->vglShape = imgShape;
  //clReleaseMemObject(img->oclPtr);
  //clReleaseMemObject(out->oclPtr);
  vglClFlush();


  //VglStrEl vglStrEl = new VglStrEl(VGL_STREL_CUBE, 2);
  //vglStrEl->print();
  int nc = 3;
  int w = 5;
  int h = 5;
  int f = 3;

  VglShape* vglShape = new VglShape(nc, w, h, f);
  //vglShape->print();
  /*
for(int k = 0; k < f; k++)
{
  for(int j = 0; j < h; j++)
  {
    for(int i = 0; i < w; i++)
    {
      for(int c = 0; c < nc; c++)
      {
        printf("c = %5d, i = %5d, j = %5d, k = %5d, pixel = %5d\n", c, i, j, k, VglShapeIndexNC3D(vglShape, c, i, j, k));
      }
    }
  }
}
  */


/*
// Testing getCoordFromIndex
int coord[11];
for(int i = 0; i < vglShape->getSize(); i++)
{
  vglShape->getCoordFromIndex(i, coord);
  printf("i = %3d, ", i);
  for (int d = 0; d <= vglShape->getNdim(); d++)
  {
    printf("%2d ", coord[d]);
  }
  printf("\n");
}
*/

printf("CHECK 1000 \n");


for(int i = 1; i <= 5; i++)
{
  VglStrEl* vglStrEl = new VglStrEl(VGL_STREL_CROSS, i);
  vglStrEl->print();
}

img->vglShape->print();


printf("CHECK 1200 \n");
printf("\n");
 printf("CHECK 1200 %s\n", "x");
printf("\n");
printf("\n");


//VglStrEl* vglStrEl = new VglStrEl(VGL_STREL_CUBE, 3);
  VglShape* vglShape2 = new VglShape(11, 1);
  VglStrEl* vglStrEl2 = new VglStrEl(strel, vglShape2);
  vglStrEl2->asVglClStrEl();

  VglStrEl* vglStrEl = new VglStrEl(VGL_STREL_CUBE, 3);


  vglClNdDilate(img, out, vglStrEl);
  vglClDownload(out);

  out->vglShape = origShape;
  vglSaveImage(out, (char*) "/tmp/teste2.png");

printf("CHECK 2000 \n");

  printf("cplusplus = %ld\n", __cplusplus);
  printf("CL_VERSION_1_0 = %d\n", CL_VERSION_1_0);
  printf("CL_VERSION_1_1 = %d\n", CL_VERSION_1_1);
  printf("CL_VERSION_1_2 = %d\n", CL_VERSION_1_2);
  //printf("CL_VERSION_2_0 = %d\n", CL_VERSION_2_0);
  return 0;
}
