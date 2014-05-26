#include "vglImage.h"
#include "vglGdcmIo.h"

int main(int argc, char *argv[])
{
  char *filename = argv[argc-2];
  char *outfilename = argv[argc-1];

  VglImage imagevgl;
  imagevgl = vglGdcmLoadDicom(filename, outfilename);
  if(argc == 3)
     int i = vglGdcmSaveDicom(imagevgl, outfilename);
  else
     int i = vglGdcmSaveDicomCompressed(imagevgl, outfilename);

  return 0;
}

