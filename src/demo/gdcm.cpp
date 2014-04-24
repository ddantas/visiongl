#include "vglImage.h"
#include "vglGdcmIo.h"

int main(int argc, char *argv[])
{
  char *filename = argv[1];
  char *outfilename = argv[2];

  VglImage imagevgl;
  imagevgl = vglGdcmLoadDicom(filename, outfilename);
  int i = vglGdcmSaveDicom(imagevgl, filename, outfilename);

  return 0;
}

