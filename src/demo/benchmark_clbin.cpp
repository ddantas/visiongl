
#ifdef __OPENCL__
//this program requires opencl

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"
#include "glsl2cpp_shaders.h"
#include "cl2cpp_BIN.h"
#include "iplImage.h"

#ifdef __OPENCV__
  #include <opencv2/imgproc/types_c.h>
  #include <opencv2/imgproc/imgproc_c.h>
  #include <opencv2/highgui/highgui_c.h>
#else
  #include <vglOpencv.h>
#endif

#include "demo/timer.h"

#include <fstream>
#include <string.h>


int main(int argc, char* argv[])
{

    if (argc != 4)
    {
        printf("\nUsage: demo_benchmark_clbin lena_1024.tiff 1000 /tmp\n\n");
        printf("In this example, will run the program for lena_1024.tiff in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
        printf("Error: Bad number of arguments = %d. 3 arguments required.\n", argc-1);
        exit(1);
    }
    //vglInit(50,50);
    vglClInit();

    int nSteps = atoi(argv[2]);
    char* inFilename = argv[1];
    char* outPath = argv[3];
    char* outFilename = (char*) malloc(strlen(outPath) + 200);

    printf("VisionGL-OpenCL-BIN on %s, %d operations\n\n", inFilename, nSteps);
	
    printf("CREATING IMAGE\n");
    IplImage* iplIn = iplLoadImage(inFilename, CV_LOAD_IMAGE_GRAYSCALE);
    VglImage* vglIn = vglCopyCreateImage(iplIn);

    printf("CHECKING NCHANNELS\n");
    if (vglIn->nChannels != 1)
    {
      fprintf(stderr, "%s: %s: Error: please load input image as grayscale.\n", __FILE__, __FUNCTION__);
      exit(1);
    }

    vglIn->vglShape->print();
    iplPrintImageInfo(vglIn->ipl);

    printf("CHECKING IF IS NULL\n");
    if (vglIn == NULL)
    {
      fprintf(stderr, "%s: %s: Error: file not found: %s\n", __FILE__, __FUNCTION__, inFilename);
    }

    printf("CREATING COPY\n");
    VglImage* vglGray   = vglCreateImage(vglIn);
    VglImage* vglThresh = vglCreateImage(cvSize(vglIn->getWidth(), vglIn->getHeight()), IPL_DEPTH_1U, 1);
    VglImage* vglBin    = vglCreateImage(cvSize(vglIn->getWidth(), vglIn->getHeight()), IPL_DEPTH_1U, 1);
    VglImage* vglRoi    = vglCreateImage(cvSize(vglIn->getWidth(), vglIn->getHeight()), IPL_DEPTH_1U, 1);
    int p;

    printf("CREATED COPY\n");
    //First call to Threshold
    TimerStart();
    vglClBinThreshold(vglIn, vglThresh, 0.5);
    vglClFlush();
    printf("Fisrt call to              Threshold:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Threshold
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinThreshold(vglIn, vglThresh, 0.5);
    }
    vglClFlush();
    printf("Time spent on %8d     Threshold:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_thresh.pbm");
    iplSavePgm(outFilename, vglThresh->ipl);

    //First call to BinToGray
    TimerStart();
    vglClBinToGray(vglThresh, vglGray);
    vglClFlush();
    printf("Fisrt call to              BinToGray:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations BinToGray
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinToGray(vglThresh, vglGray);
    }
    vglClFlush();
    printf("Time spent on %8d     BinToGray:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglGray, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_togray.pbm");
    iplSavePgm(outFilename, vglGray->ipl);

    //First call to Not
    TimerStart();
    vglClBinNot(vglThresh, vglBin);
    vglClFlush();
    printf("Fisrt call to                    Not:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Not
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinNot(vglThresh, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d           Not:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_not.pbm");
    iplSavePgm(outFilename, vglBin->ipl);

    float strEl[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    //First call to Dilate
    TimerStart();
    vglClBinDilate(vglThresh, vglBin, strEl, 3, 3);
    vglClFlush();
    printf("Fisrt call to                 Dilate:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Dilate
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinDilate(vglThresh, vglBin, strEl, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d        Dilate:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_dilate.pbm");
    iplSavePgm(outFilename, vglBin->ipl);

    //First call to Erode
    TimerStart();
    vglClBinErode(vglThresh, vglBin, strEl, 3, 3);
    vglClFlush();
    printf("Fisrt call to                  Erode:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Erode
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinErode(vglThresh, vglBin, strEl, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d         Erode:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_erode.pbm");
    iplSavePgm(outFilename, vglBin->ipl);

    //First call to Roi
    TimerStart();
    vglClBinRoi(vglRoi, 4, 4, 300, 300);
    vglClFlush();
    printf("Fisrt call to                    Roi:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Roi
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinRoi(vglRoi, 4, 4, 300, 300);
    }
    vglClFlush();
    printf("Time spent on %8d           Roi:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglRoi, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_roi.pbm");
    iplSavePgm(outFilename, vglRoi->ipl);

    //First call to Max
    TimerStart();
    vglClBinMax(vglThresh, vglRoi, vglBin);
    vglClFlush();
    printf("Fisrt call to                    Max:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Max
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinMax(vglThresh, vglRoi, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d           Max:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_max.pbm");
    iplSavePgm(outFilename, vglBin->ipl);

    //First call to Min
    TimerStart();
    vglClBinMin(vglThresh, vglRoi, vglBin);
    vglClFlush();
    printf("Fisrt call to                    Min:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Min
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinMin(vglThresh, vglRoi, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d           Min:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_min.pbm");
    iplSavePgm(outFilename, vglBin->ipl);

    //First call to Sub
    TimerStart();
    vglClBinSub(vglThresh, vglRoi, vglBin);
    vglClFlush();
    printf("Fisrt call to                    Sub:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Sub
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinSub(vglThresh, vglRoi, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d           Sub:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_sub.pbm");
    iplSavePgm(outFilename, vglBin->ipl);

    //First call to Copy GPU->GPU
    TimerStart();
    vglClBinCopy(vglThresh, vglBin);
    vglClFlush();
    printf("Fisrt call to          Copy GPU->GPU:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->GPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBinCopy(vglThresh, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d Copy GPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_copy.pbm");
    iplSavePgm(outFilename, vglBin->ipl);

    //First call to Copy CPU->GPU
    vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
    TimerStart();
    vglSetContext(vglThresh, VGL_RAM_CONTEXT);
    vglClUpload(vglThresh);
    vglClFlush();
    printf("Fisrt call to          Copy CPU->GPU:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy CPU->GPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglSetContext(vglThresh, VGL_RAM_CONTEXT);
        vglClUpload(vglThresh);
    }
    vglClFlush();
    printf("Time spent on %8d Copy CPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_upload.pbm");
    iplSavePgm(outFilename, vglThresh->ipl);

    //First call to Copy GPU->CPU
    vglCheckContext(vglThresh, VGL_CL_CONTEXT);
    TimerStart();
    vglSetContext(vglThresh, VGL_CL_CONTEXT);
    vglClDownload(vglThresh);
    vglClFlush();
    printf("Fisrt call to          Copy GPU->CPU:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->CPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglSetContext(vglThresh, VGL_CL_CONTEXT);
        vglClDownload(vglThresh);
    }
    vglClFlush();
    printf("Time spent on %8d Copy GPU->CPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_clbin_download.pbm");
    iplSavePgm(outFilename, vglThresh->ipl);

    //flush
    vglClFlush();
    return 0;

}

#endif
