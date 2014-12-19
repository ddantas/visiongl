#include <vglImage.h>
#include <vglTiffIo.h>
#include <vglGdcmIo.h>


int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads a tiff file and save a copy using the GDCM library. \n\
    Usage is as follows:\n\
\n\
demo_tiff <input file> <output file>\n\
";

  if (argc < 3)
  {
    printf("%s", usage);
    exit(1);
  }

  char* inFilename = argv[1]; // name of the input file
  char* outFilename = argv[2]; // name of the output file
  VglImage* vgltiff = vglLoadTiff(inFilename);
  
  printf("\nwidth = %d\nheight = %d\nlayers = %d\ndepth = %d\nnChannels = %d\n\n", vgltiff->shape[VGL_WIDTH], vgltiff->shape[VGL_HEIGHT], vgltiff->shape[VGL_LENGTH], vgltiff->depth, vgltiff->nChannels);

  vglPrintTiffInfo(inFilename);

  int r = vglGdcmSaveDicomUncompressed(vgltiff, outFilename); 
}
