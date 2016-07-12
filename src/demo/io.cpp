#include <vglImage.h>

//IplImage, cvLoadImage
#ifdef __OPENCV__
  #include <opencv2/highgui/highgui_c.h>
#else
  #include <vglOpencv.h>
#endif


int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads a file and saves a copy. Usage is as follows:\n\
\n\
    demo_io <input file> <output file> <option>\n\
    option:\n\
    -1 -> Load image as is.\n\
     0 -> Load image as grayscale.\n\
     1 -> Load image as BGR.\n\
\n";

  if (argc < 4)
  {
    printf("%s", usage);
    exit(1);
  }

  char *inFilename = argv[1]; // name of the input file
  char *outFilename = argv[2]; // name of the output file
  int option = atoi(argv[3]); // color option

  IplImage* iplImage;

  printf("\noption = %d\n\n", option);
  int i;
  switch(option)
  {
     case -1:
     case 0:
     case 1:
       iplImage = cvLoadImage(inFilename, option);
       i = cvSaveImage(outFilename, iplImage);
       break;

     default:
       iplImage = cvLoadImage(inFilename);
       i = cvSaveImage(outFilename, iplImage);
       break;
  }
  return 0;
  
}
