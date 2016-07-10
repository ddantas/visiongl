
#include "vglImage.h"
#include "vglDcmtkIo.h"
#include "glsl2cpp_shaders.h"


int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
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
  char *compressType = argv[argc-1];

  int size_filename = strlen(argv[1]+1);
  char extension[10];
  strcpy(extension, &filename[size_filename-3]);

  VglImage* imagevgl;

  for(int i = 0; i < strlen(extension); i++)
    extension[i] = toupper(extension[i]);
  
  if(strcmp(extension, ".DCM") == 0) 
    imagevgl = vglDcmtkLoadDicom(filename);   
  else
    if(strcmp(extension, ".DCM") != 0)
    {
      int   imgIFirst = atoi(argv[3]);
      int   imgILast  = atoi(argv[4]);
      printf("\nfilename: %s\n", filename);
      imagevgl = vglLoad3dImage(filename, imgIFirst, imgILast); 
    }
  
  vglPrintImageInfo(imagevgl);
  //vglPrintImageData(&imagevgl);

  //vglInit(1, 1);
  //vglUpload(imagevgl);
  //vgl3dNot(imagevgl, imagevgl); 
  // The GLSL 3d version of this shader works only in the first frame. 
  // Please use the OpenCL version vglCl3dNot.
  //vglDownload(imagevgl);

  
  if(strcmp(compressType, "-c") != 0)
    int i = vglDcmtkSaveDicomUncompressed(outfilename, imagevgl);
  else
    int i = vglDcmtkSaveDicomCompressed(outfilename, imagevgl);

  return 0;
}
