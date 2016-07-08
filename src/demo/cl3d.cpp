
#include "vglImage.h"
#include "vglClImage.h"
#include "cl2cpp_shaders.h"
#include "glsl2cpp_shaders.h"

#include <string.h>

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
  sprintf(outfilename, "%s/%%05d.ppm", outfolder);
  printf("outfilename = %s\n", outfilename);

  vglInit(500,500);
  vglClInit();
  VglImage* img = vglLoad3dImage(infilename, i_0, i_n);

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


  //vglCl3dNot(img, out);
  //vglCl3dConvolution(img, out, blur333,3,3,3);
  //vglCl3dBlurSq3(img, out);
  //vglCl3dThreshold(img, out, 0.5);
  vglCl3dDilate(img, out, strel, 1, 1, 5);
  vglClDownload(out);

  if (out->nChannels == 4)
  {
    vglNdarray4To3Channels(out);
  }

  vglSave3dImage(outfilename, out, i_0, i_n);
  //clReleaseMemObject(img->oclPtr);
  //clReleaseMemObject(out->oclPtr);
  vglClFlush();

  printf("Output written to %s\n", outfilename);


  return 0;
}
