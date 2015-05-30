
#include <vglImage.h>
#include <vglGdcmIo.h>
#include <vglDcmtkIo.h>

//toupper
#include <ctype.h>

int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads an image and displays information\n\
about it. Usage is as follows:\n\
\n\
image_info <input file>\n\
\n\
    To print the image data, provide the parameter -p as follows \n\
image_info <input file> -p\n\
";

  if (argc < 2)
  {
    printf("%s", usage);
    exit(1);
  }

  char *filename = argv[1]; // name of the input file

  int size_filename = strlen(argv[1]+1);
  char extension[10];
  strcpy(extension, &filename[size_filename-3]);

  VglImage* imagevgl;

  for(int i = 0; i < strlen(extension); i++)
    extension[i] = toupper(extension[i]);

  if(strcmp(extension, ".DCM") == 0)
  {
#ifdef __GDCM__
    imagevgl = vglGdcmLoadDicom(filename);
#elif defined __DCMTK__
    imagevgl = vglDcmtkLoadDicom(filename);
#else
    fprintf(stderr, "%s: %s: Error: Please recompile the library with GDCM or DCMTK support loading DICOM images\n", __FILE__, __FUNCTION__);
    exit(1);
#endif
  }
  else
  {
    imagevgl = vglLoadImage(filename); 
  }
  
  vglPrintImageInfo(imagevgl);

  if (  ( argc >= 3 ) && ( strcmp(argv[2], "-p") == 0 )  )
  {
    vglPrintImageData(imagevgl);
  }

  return 0;
}
