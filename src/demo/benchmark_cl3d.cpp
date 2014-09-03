
#ifdef __OPENCL__
//this program requires opencl

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "demo/timer.h"

#include <fstream>

int main(int argc, char* argv[])
{

    if (argc != 6)
    {
        printf("\nUsage: demo_benchmark_cl3d 0003.dcm.%%d.PGM 0 16 1000 /tmp\n\n");
        printf("In this example, will run the program for 0003.dcm.{0..16}.PGM in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
        printf("Error: Bad number of arguments = %d. 5 arguments required.\n", argc-1);
        exit(1);
    }

    vglInit(50,50);
    vglClInit();

    int nSteps = atoi(argv[4]);
    char* inFilename = argv[1];
    int   imgIFirst = atoi(argv[2]);
    int   imgILast  = atoi(argv[3]);
    char* outPath = argv[5];
    char* outFilename = (char*) malloc(strlen(outPath) + 200);

    printf("VisionGL-OpenCL3D on %s, %d operations\n\n", inFilename, nSteps);
       
    printf("CREATING IMAGE\n");
    VglImage* img = vglLoad3dImage(inFilename, imgIFirst, imgILast);
    
    printf("CHECKING NCHANNELS\n");
    if (img->nChannels == 3)
    {
        printf("NCHANNELS = 3\n");
        if (img->ndarray)
        {
            printf("NDARRAY not null\n");
            vglNdarray3To4Channels(img);
        }
        else
        {
            printf("NDARRAY IS null\n");
            vglIpl3To4Channels(img);
        }
    }

    if (img == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s", str.c_str());
    }
    VglImage* out = vglCreate3dImage(cvSize(img->shape[VGL_WIDTH],img->shape[VGL_HEIGHT]), img->depth, img->nChannels, img->shape[VGL_LENGTH], 0);

    printf("IMAGE CREATED\n");
    vglPrintImageInfo(img, (char*) "IMG");
    vglPrintImageInfo(out, (char*) "OUT");

    //First call to Blur 3x3x3
    TimerStart();
    vglCl3dBlurSq3(img, out);
    printf("First call to          Blur 3x3x3:              %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Blur 3x3
    int p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dBlurSq3(img, out);
    }
    printf("Time spent on %8d Blur 3x3x3:              %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_blur333_%03d.jpg");
    vglSaveImage(out, outFilename, imgIFirst, imgILast);

    // Convolution kernels
    // 3x3x3 mask for mean filter
    float* mask333 = (float*) malloc(sizeof(float)*27);
    for(int i = 0; i < 27; i++)
        mask333[i] = 1.0f/27.0f;
    // 5x5x5 mask for mean filter
    float* mask555 = (float*) malloc(sizeof(float)*125);
    for(int i = 0; i < 125; i++)
        mask555[i] = 1.0f/125.0f;

    //First call to Convolution 3x3x3
    TimerStart();
    vglCl3dConvolution(img, out, (float*) mask333, 3, 3, 3);
    printf("First call to          Convolution 3x3x3:       %s\n", getTimeElapsedInSeconds());

    //Total time spent on n operations Convolution 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dConvolution(img, out, (float*) mask333, 3, 3, 3);
    }
    printf("Time spent on %8d Convolution 3x3x3:       %s \n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_conv333_%03d.jpg");
    vglSaveImage(out, outFilename, imgIFirst, imgILast);

    //Total time spent on n operations Convolution 5x5
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dConvolution(img, out, (float*) mask555, 5, 5, 5);
    }
    printf("Time spent on %8d Convolution 5x5x5:       %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_conv555_%03d.jpg");
    vglSaveImage(out, outFilename, imgIFirst, imgILast);

    //First call to Erode 3x3x3
    float erodeMask[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 1};
    TimerStart();
    vglCl3dErosion(img, out, erodeMask, 3, 3, 3);
    printf("First call to          Erode 3x3x3:             %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Erode 3x3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dErosion(img, out, erodeMask, 3, 3, 3);
    }
    printf("Time spent on %8d Erode 3x3x3:             %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_erode333_%03d.jpg");
    vglSaveImage(out, outFilename, imgIFirst, imgILast);

    //First call to Invert
    TimerStart();
    vglCl3dNot(img, out);
    printf("Fisrt call to          Invert:                  %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Invert
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dNot(img, out);
    }
    printf("Time spent on %8d Invert:                  %s\n", nSteps, getTimeElapsedInSeconds());
	
    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_invert_%03d.jpg");
    vglSaveImage(out, outFilename, imgIFirst, imgILast);

    //First call to Threshold
    TimerStart();
    vglCl3dThreshold(img, out, 127.0);
    printf("Fisrt call to          Threshold:               %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Threshold
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dThreshold(img, out, 127.0);
    }
    printf("Time spent on %8d Threshold:               %s\n", nSteps, getTimeElapsedInSeconds());
	
    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_thresh_%03d.jpg");
    vglSaveImage(out, outFilename, imgIFirst, imgILast);

    //First call to Copy GPU->GPU
    TimerStart();
    vglCl3dCopy(img,out);
    printf("First call to          Copy GPU->GPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->GPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglCl3dCopy(img, out);
    }
    printf("Time spent on %8d copy GPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_gpuCopy_%03d.jpg");
    vglSaveImage(out, outFilename, imgIFirst, imgILast);

    //First call to Copy CPU->GPU
    p = 0;
    vglClUpload(img);
    printf("First call to          Copy CPU->GPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy CPU->GPU
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClUpload(img);
    }
    printf("Time spent on %8d copy CPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(img, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_upload_%03d.jpg");
    vglSaveImage(img, outFilename, imgIFirst, imgILast);

    //First call to Copy GPU->CPU
    TimerStart();
    vglClDownload(img);
    printf("First call to          Copy GPU->CPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->CPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClDownload(img);
    }
    printf("Time spent on %8d copy GPU->CPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(img, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl3d_download_%03d.jpg");
    vglSaveImage(img, outFilename, imgIFirst, imgILast);

    //flush
    vglClFlush();
    return 0;
}

#endif
