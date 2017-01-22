#include <vglImage.h>
#include <vglContext.h>
#include <vglClImage.h>
#include <cl2cpp_BIN.h>
#include <cl2cpp_shaders.h>
#include <iplImage.h>

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
  VglImage* vglRoi;
  VglImage* vglConway0;
  VglImage* vglConway1;
  VglImage* vglBin;

  float seSquare[9] = {0,0,0,
                       0,1,1,
                       0,1,0};

  vglClInit();

  {
      iplInput   = iplLoadImage(inFilename, CV_LOAD_IMAGE_GRAYSCALE);
      vglInput   = vglCopyCreateImage(iplInput);
      vglGray    = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_8U, 1);
      vglThresh  = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglToGray  = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_8U, 1);
      vglNot     = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglDilate  = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglErode   = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglRoi     = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglConway0 = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglConway1 = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);
      vglBin     = vglCreateImage(cvSize(vglInput->getWidth(), vglInput->getHeight()), IPL_DEPTH_1U, 1);

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
      vglCheckContext(vglThresh, VGL_RAM_CONTEXT);      // Transfer image to RAM
      iplSavePgm(outFilename, vglThresh->ipl);          // Save image as PBM, 8bpp, signature P4.

      vglClBinToGray(vglThresh, vglToGray);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_to_gray.pgm");
      vglSaveImage(outFilename, vglToGray);

      vglClBinNot(vglThresh, vglNot);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_not.pbm");
      vglCheckContext(vglNot, VGL_RAM_CONTEXT);
      iplSavePgm(outFilename, vglNot->ipl);

      vglClBinDilate(vglThresh, vglDilate, seSquare, 3, 3);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_dilate.pbm");
      vglCheckContext(vglDilate, VGL_RAM_CONTEXT);
      iplSavePgm(outFilename, vglDilate->ipl);

      vglClBinErode(vglThresh, vglErode, seSquare, 3, 3);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_erode.pbm");
      vglCheckContext(vglErode, VGL_RAM_CONTEXT);
      iplSavePgm(outFilename, vglErode->ipl);

      vglClBinRoi(vglRoi, 5, 40, 256, 256);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_roi.pbm");
      vglCheckContext(vglRoi, VGL_RAM_CONTEXT);
      iplSavePgm(outFilename, vglRoi->ipl);

      vglClBinMax(vglThresh, vglRoi, vglBin);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_max.pbm");
      vglCheckContext(vglBin, VGL_RAM_CONTEXT);
      iplSavePgm(outFilename, vglBin->ipl);

      vglClBinMin(vglThresh, vglRoi, vglBin);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_min.pbm");
      vglCheckContext(vglBin, VGL_RAM_CONTEXT);
      iplSavePgm(outFilename, vglBin->ipl);

      vglClBinSub(vglThresh, vglRoi, vglBin);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_sub.pbm");
      vglCheckContext(vglBin, VGL_RAM_CONTEXT);
      iplSavePgm(outFilename, vglBin->ipl);


      vglClBinCopy(vglThresh, vglConway0);
      sprintf(outFilename, "%s%s", outPath, "/demo_bin_conway.pbm");
      vglCheckContext(vglConway0, VGL_RAM_CONTEXT);
      iplSavePgm(outFilename, vglConway0->ipl);

      vglClBinCopy(vglThresh, vglConway0);
      for(int i = 0; i <= iter; i++)
      {
        sprintf(outFilename, "%s%s%03d%s", outPath, "/demo_bin_conway", i, ".pbm");
        vglCheckContext(vglConway0, VGL_RAM_CONTEXT);
        iplSavePgm(outFilename, vglConway0->ipl);

        vglClBinConway(vglConway0, vglConway1);
        vglClBinCopy(vglConway1, vglConway0);
      }

  }

  return 0;  
}
