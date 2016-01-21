
#include "vglImage.h"
#include "vglContext.h"
#include "kernel2cu_shaders.h"


int main(int argc, char *argv[])
{
  char* usage = (char*)"\n\
    This program reads an image file, finds its negative using cuda and saves \n\
a copy. Usage is as follows:\n\
\n\
demo_cuda <input file> <output file>\n\
";

  if (argc < 3)
  {
    printf("%s", usage);
    exit(1);
  }

  char *filename = argv[1]; // name of the input file
  char *outfilename = argv[2]; // name of the output file

  vglInit(1, 1);

  int size_filename = strlen(argv[1]+1);
  char extension[10];
  strcpy(extension, &filename[size_filename-3]);

  VglImage* img_in;
  VglImage* img_out;

  img_in  = vglLoadImage(filename); 
  img_out = vglCreateImage(img_in); 
  
  printf("Calling CudaInvertOnPlace\n");
  vglCudaInvertOnPlace(img_in); 
  printf("Calling CudaCopy\n");
  vglCudaCopy(img_in, img_out);

  //vglSaveImage calls vglCheckContext to transfer image to RAM.
  //vglCheckContext(img_out, VGL_RAM_CONTEXT);

  printf("Saving result to %s\n", outfilename);
  vglSaveImage(img_out, outfilename);

  return 0;
}
