#include <vglImage.h>
#include <vglContext.h>
#include <vglClImage.h>
#include <cl2cpp_BIN.h>
#include <cl2cpp_shaders.h>

// IplImage, cvLoadImage
#ifdef __OPENCV__
  #include <opencv2/highgui/highgui_c.h>
  #include <opencv2/imgproc/imgproc_c.h>
#else
  #include <vglOpencv.h>
#endif

// strlen
#include <string.h>

int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    This program reads a file and saves a copy. Usage is as follows:\n\
\n\
    demo_bin <input file> <output file> <num. conway>\n\
\n";

  if (argc < 3)
  {
    printf("%s", usage);
    exit(1);
  }

  char *inFilename = argv[1];  // name of the input file
  char *outPath    = argv[2];  // name of the output path
  int   iter       = 20;
  char* outFilename = (char*) malloc(strlen(outPath) + 200);

  if (argc >= 3)
  {
    iter = atoi(argv[3]);
  }

  IplImage* iplInput;
  VglImage* vglInput;
  VglImage* vglGray;
  VglImage* vglThresh;
  VglImage* vglToGray;
  VglImage* vglNot;
  VglImage* vglDilate;
  VglImage* vglErode;
  VglImage* vglConway0;
  VglImage* vglConway1;

  float seSquare[9] = {0,0,0,
                       0,1,1,
                       0,1,0};

  vglClInit();

  {
      iplInput   = cvLoadImage(inFilename, CV_LOAD_IMAGE_GRAYSCALE);
      vglInput   = vglCopyCreateImage(iplInput);
      vglGray    = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_8U, 1);
      vglThresh  = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglToGray  = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_8U, 1);
      vglNot     = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglDilate  = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglErode   = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglConway0 = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglConway1 = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);

      if (vglInput->depth == IPL_DEPTH_1U)
      {
        vglClBinToGray(vglInput, vglGray);
      }
      else if (vglInput->depth == IPL_DEPTH_8U)
      {
        vglGray = vglInput;
      }

      sprintf(outFilename, "%s%s", outPath, "/demo_bin_input.pgm");
      vglSaveImage(outFilename, vglGray);

      vglClBinThreshold(vglGray, vglThresh, .5);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_thresh.pbm");
      vglSaveImage(outFilename, vglThresh);

      vglClBinToGray(vglThresh, vglToGray);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_to_gray.pgm");
      vglSaveImage(outFilename, vglToGray);

      vglClBinNot(vglThresh, vglNot);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_not.pbm");
      vglSaveImage(outFilename, vglNot);

      vglClBinDilate(vglNot, vglDilate, seSquare, 3, 3);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_dilate.pbm");
      vglSaveImage(outFilename, vglDilate);

      vglClBinErode(vglDilate, vglErode, seSquare, 3, 3);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_erode.pbm");
      vglSaveImage(outFilename, vglErode);

      vglClBinCopy(vglThresh, vglConway0);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_lixo.pbm");
      vglSaveImage(outFilename, vglThresh);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_conway.pbm");
      vglSaveImage(outFilename, vglConway0);

      {
        vglClBinCopy(vglThresh, vglConway0);
        for(int i = 0; i <= iter; i++)
        {
          vglClBinConway(vglConway0, vglConway1);
          sprintf(outFilename, "%s%s%d%s", outPath, "/demo_bin_conway", i, ".pbm");
          vglSaveImage(outFilename, vglConway0);
          vglClBinCopy(vglConway1, vglConway0);
        }
      }

  }

  return 0;  
}
