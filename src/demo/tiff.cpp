#include <vglImage.h>
#include <vglTiffIo.h>
#include <vglGdcmIo.h>


int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads a tiff file and save a copy using the libTIFF library. \n\
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
  int i = vglPrintTiffInfo(inFilename);
  VglImage* vgltiff = vglLoadTiff(inFilename);
  
  if(vgltiff)
  {
    printf("\nwidth = %d\nheight = %d\nlayers = %d\ndepth = %d\nnChannels = %d\n\n", vgltiff->getWidth(), vgltiff->getHeight(), vgltiff->getLength(), vgltiff->depth, vgltiff->nChannels);

    //int r = vglGdcmSaveDicomUncompressed(vgltiff, outFilename); 
    int r = vglSaveTiff(vgltiff, outFilename); 
    return 0;
  }
  return 1;
}
