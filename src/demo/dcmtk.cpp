
#include "vglImage.h"
#include "vglDcmtkIo.h"
#include "glsl2cpp_shaders.h"


int main(int argc, char *argv[])
{
  char* usage = "\n\
    This program reads a dicom file using the DCMTK library and saves \n\
a copy. Usage is as follows:\n\
\n\
demo_dcmtk <input file> <output file>\n\
\n\
    To save to a compressed dicom file:\n\
\n\
demo_dcmtk <input file> <output file> -c\n\
";

  if (argc < 3)
  {
    printf("%s", usage);
    exit(1);
  }

  char *filename = argv[1]; // name of the input file
  char *outfilename = argv[2]; // name of the output file

  VglImage imagevgl;

  imagevgl = vglDcmtkLoadDicom(filename);

  vglPrintImageInfo(&imagevgl);
  //vglPrintImageData(&imagevgl);

  vglInit(1, 1);
  vglUpload(&imagevgl);
  vgl3dNot(&imagevgl, &imagevgl); 
  // The GLSL 3d version of this shader works only in the first frame. 
  // Please use the OpenCL version vglCl3dNot.
  vglDownload(&imagevgl);

  if(argc == 3)
     int i = vglDcmtkSaveDicom(imagevgl, outfilename);
  else
     int i = vglDcmtkSaveDicomCompressed(imagevgl, outfilename);

  return 0;
}
