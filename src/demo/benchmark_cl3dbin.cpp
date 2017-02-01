
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

    if (argc != 6)
    {
        printf("\nUsage: demo_benchmark_cl3dbin 0003.dcm.%%d.PGM 0 16 1000 /tmp\n\n");
        printf("In this example, will run the program for 0003.dcm.{0..16}.PGM in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
        printf("Error: Bad number of arguments = %d. 5 arguments required.\n", argc-1);
        exit(1);
    }
    //vglInit(50,50);
    vglClInit();

    int nSteps = atoi(argv[4]);
    char* inFilename = argv[1];
    int   imgIFirst = atoi(argv[2]);
    int   imgILast  = atoi(argv[3]);
    char* outPath = argv[5];
    char* outFilename = (char*) malloc(strlen(outPath) + 200);

    printf("VisionGL-OpenCL-3D-BIN on %s, %d operations\n\n", inFilename, nSteps);
	
    printf("CREATING IMAGE\n");
    VglImage* vglIn = vglLoad3dImage(inFilename, imgIFirst, imgILast);

    printf("CHECKING NCHANNELS\n");
    if (vglIn->nChannels != 1)
    {
      fprintf(stderr, "%s: %s: Error: please load input image as grayscale.\n", __FILE__, __FUNCTION__);
      exit(1);
    }

    vglIn->vglShape->print();

    printf("CHECKING IF IS NULL\n");
    if (vglIn == NULL)
    {
      fprintf(stderr, "%s: %s: Error: file not found: %s\n", __FILE__, __FUNCTION__, inFilename);
    }

    printf("CREATING COPY\n");
    int ndim = 3;
    int shape[VGL_MAX_DIM];
    shape[VGL_SHAPE_NCHANNELS] = 1;
    shape[VGL_SHAPE_WIDTH]     = vglIn->getWidth();
    shape[VGL_SHAPE_HEIGHT]    = vglIn->getHeight();
    shape[VGL_SHAPE_LENGTH]    = vglIn->getLength();

    VglImage* vglGray   = vglCreateImage(vglIn);
    VglImage* vglThresh = vglCreateImage(shape, IPL_DEPTH_1U, ndim);
    VglImage* vglBin    = vglCreateImage(shape, IPL_DEPTH_1U, ndim);
    VglImage* vglBin2   = vglCreateImage(shape, IPL_DEPTH_1U, ndim);
    VglImage* vglRoi    = vglCreateImage(shape, IPL_DEPTH_1U, ndim);
    VglImage* vglDil    = vglCreateImage(shape, IPL_DEPTH_1U, ndim);
    int p;

    printf("CREATED COPY\n");
    //First call to Threshold
    TimerStart();
    vglCl3dBinThreshold(vglIn, vglThresh, 0.2);
    vglClFlush();
    printf("Fisrt call to                  Threshold:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Threshold
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinThreshold(vglIn, vglThresh, 0.2);
    }
    vglClFlush();
    printf("Time spent on %8d         Threshold:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_thresh_%03d.pbm");
    vglSave3dImage(outFilename, vglThresh, 0);

    //First call to BinToGray
    TimerStart();
    vglCl3dBinToGray(vglThresh, vglGray);
    vglClFlush();
    printf("Fisrt call to                  BinToGray:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations BinToGray
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinToGray(vglThresh, vglGray);
    }
    vglClFlush();
    printf("Time spent on %8d         BinToGray:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglGray, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_togray_%03d.pgm");
    vglSave3dImage(outFilename, vglGray, 0);

    //First call to Not
    TimerStart();
    vglCl3dBinNot(vglThresh, vglBin);
    vglClFlush();
    printf("Fisrt call to                        Not:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Not
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinNot(vglThresh, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d               Not:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_not_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    float seCube[27]  = { 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 1, };
    float seCross[27] = { 0, 0, 0, 0, 1, 0, 0, 0, 0,
                          0, 1, 0, 1, 1, 1, 0, 1, 0,
                          0, 0, 0, 0, 1, 0, 0, 0, 0, };
    float seAngle[27] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 1, 1, 0, 1, 0,
                          0, 0, 0, 0, 1, 0, 0, 0, 0, };
    float seSep[3]    = { 1, 1, 1 };

    //First call to Dilate
    TimerStart();
    vglCl3dBinDilate(vglThresh, vglBin, seCube, 3, 3, 3);
    vglClFlush();
    printf("First call to           Dilation 3D cube:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Dilate cube
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinDilate(vglThresh, vglDil, seCube, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d  Dilation 3D cube:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglDil, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_dilate_std_cube_%03d.pbm");
    vglSave3dImage(outFilename, vglDil, 0);

    //Total time spent on n operations Dilate sep.
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinDilate(vglThresh, vglBin, seSep, 3, 1, 1);
        vglCl3dBinDilate(vglBin, vglBin2, seSep, 1, 3, 1);
        vglCl3dBinDilate(vglBin2, vglBin, seSep, 1, 1, 3);
    }
    vglClFlush();
    printf("Time spent on %8d  Dilation 3D sep.:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_dilate_std_sep_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Dilate cross
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinDilate(vglThresh, vglBin, seCross, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d Dilation 3D cross:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_dilate_std_cube_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Dilate angle
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinDilate(vglThresh, vglBin, seAngle, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d Dilation 3D angle:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_dilate_std_angle_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //First call to Dilate pack
    TimerStart();
    vglCl3dBinDilatePack(vglThresh, vglBin, seCube, 3, 3, 3);
    vglClFlush();
    printf("First call to           Dila pac 3D cube:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Dilate pack cube
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinDilatePack(vglThresh, vglBin, seCube, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d  Dila pac 3D cube:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_dilate_pack_cube_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Dilate pack sep.
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinDilatePack(vglThresh, vglBin, seSep, 3, 1, 1);
        vglCl3dBinDilatePack(vglBin, vglBin2, seSep, 1, 3, 1);
        vglCl3dBinDilatePack(vglBin2, vglBin, seSep, 1, 1, 3);
    }
    vglClFlush();
    printf("Time spent on %8d  Dila pac 3D sep.:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_dilate_pack_sep_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Dilate pack cross
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinDilatePack(vglThresh, vglBin, seCross, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d Dila pac 3D cross:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_dilate_pack_cross_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Dilate pack angle
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinDilatePack(vglThresh, vglBin, seAngle, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d Dila pac 3D angle:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_dilate_pack_angle_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //First call to Erode
    TimerStart();
    vglCl3dBinErode(vglThresh, vglBin, seCube, 3, 3, 3);
    vglClFlush();
    printf("Fisrt call to                      Erode:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Erode cube
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinErode(vglDil,    vglBin, seCube, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d   Erosion 3D cube:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_erode_std_cube_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Erode sep.
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinErode(vglDil,    vglBin, seSep, 3, 1, 1);
        vglCl3dBinErode(vglBin, vglBin2, seSep, 1, 3, 1);
        vglCl3dBinErode(vglBin2, vglBin, seSep, 1, 1, 3);
    }
    vglClFlush();
    printf("Time spent on %8d   Erosion 3D sep.:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_erode_std_sep_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Erode cross
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinErode(vglDil,    vglBin, seCross, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d  Erosion 3D cross:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_erode_std_cross_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Erode angle
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinErode(vglDil,    vglBin, seAngle, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d  Erosion 3D angle:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_erode_std_angle_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //First call to Erode pack
    TimerStart();
    vglCl3dBinErodePack(vglThresh, vglBin, seCube, 3, 3, 3);
    vglClFlush();
    printf("Fisrt call to                    Ero pac:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Erode pack cube
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinErodePack(vglDil,    vglBin, seCube, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d   Ero pac 3D cube:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_erode_pack_cube_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Erode pack sep.
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinErodePack(vglDil,    vglBin, seSep, 3, 1, 1);
        vglCl3dBinErodePack(vglBin, vglBin2, seSep, 1, 3, 1);
        vglCl3dBinErodePack(vglBin2, vglBin, seSep, 1, 1, 3);
    }
    vglClFlush();
    printf("Time spent on %8d   Ero pac 3D sep.:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_erode_pack_sep_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Erode pack cross
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinErodePack(vglDil,    vglBin, seCross, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d  Ero pac 3D cross:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_erode_pack_cross_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //Total time spent on n operations Erode pack angle
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinErodePack(vglDil,    vglBin, seAngle, 3, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d  Ero pac 3D angle:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_erode_pack_angle_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //First call to Roi
    TimerStart();
    vglCl3dBinRoi(vglRoi, 4, 4, 4, 300, 300, 10);
    vglClFlush();
    printf("First call to                        Roi:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Roi
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinRoi(vglRoi, 4, 4, 4, 300, 300, 10);
    }
    vglClFlush();
    printf("Time spent on %8d               Roi:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglRoi, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_roi_%03d.pbm");
    vglSave3dImage(outFilename, vglRoi, 0);

    //First call to Max
    TimerStart();
    vglCl3dBinMax(vglThresh, vglRoi, vglBin);
    vglClFlush();
    printf("First call to                        Max:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Max
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinMax(vglThresh, vglRoi, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d               Max:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_max_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //First call to Min
    TimerStart();
    vglCl3dBinMin(vglThresh, vglRoi, vglBin);
    vglClFlush();
    printf("First call to                        Min:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Min
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinMin(vglThresh, vglRoi, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d               Min:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_min_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //First call to Sub
    TimerStart();
    vglCl3dBinSub(vglThresh, vglRoi, vglBin);
    vglClFlush();
    printf("First call to                        Sub:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Sub
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinSub(vglThresh, vglRoi, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d               Sub:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_sub_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //First call to Copy GPU->GPU
    TimerStart();
    vglCl3dBinCopy(vglThresh, vglBin);
    vglClFlush();
    printf("First call to              Copy GPU->GPU:           %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->GPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBinCopy(vglThresh, vglBin);
    }
    vglClFlush();
    printf("Time spent on %8d     Copy GPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglBin, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_copy_%03d.pbm");
    vglSave3dImage(outFilename, vglBin, 0);

    //First call to Copy CPU->GPU
    vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
    TimerStart();
    vglSetContext(vglThresh, VGL_RAM_CONTEXT);
    vglClUpload(vglThresh);
    vglClFlush();
    printf("First call to              Copy CPU->GPU:           %s\n", getTimeElapsedInSeconds());
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
    printf("Time spent on %8d     Copy CPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_upload_%03d.pbm");
    vglSave3dImage(outFilename, vglThresh, 0);

    //First call to Copy GPU->CPU
    vglCheckContext(vglThresh, VGL_CL_CONTEXT);
    TimerStart();
    vglSetContext(vglThresh, VGL_CL_CONTEXT);
    vglClDownload(vglThresh);
    vglClFlush();
    printf("First call to              Copy GPU->CPU:           %s\n", getTimeElapsedInSeconds());
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
    printf("Time spent on %8d     Copy GPU->CPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3dbin_download_%03d.pbm");
    vglSave3dImage(outFilename, vglThresh, 0);

    //flush
    vglClFlush();
    return 0;

}

#endif
