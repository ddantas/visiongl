
#ifdef __OPENCL__
//this program requires opencl

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"
#include "glsl2cpp_shaders.h"

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "demo/timer.h"

#include <fstream>

int main(int argc, char* argv[])
{

    if (argc != 4)
    {
        printf("\nUsage: demo_benchmark_cl lena_1024.tiff 1000 /tmp\n\n");
        printf("In this example, will run the program for lena_1024.tiff in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
        printf("Error: Bad number of arguments = %d. 3 arguments required.\n", argc-1);
        exit(1);
    }
    vglInit(50,50);
    vglClInit();

    int nSteps = atoi(argv[2]);
    char* inFilename = argv[1];
    char* outPath = argv[3];
    char* outFilename = (char*) malloc(strlen(outPath) + 200);

    printf("VisionGL-OpenCL on %s, %d operations\n\n", inFilename, nSteps);
	
    printf("CREATING IMAGE\n");
    VglImage* img = vglLoadImage(inFilename, CV_LOAD_IMAGE_UNCHANGED, 0);

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

    img->vglShape->print();
    iplPrintImageInfo(img->ipl);

    printf("CHECKING IF IS NULL\n");
    if (img == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s", str.c_str());
    }

    printf("CREATING COPY\n");
    VglImage* out = vglCreateImage(img);

    printf("CREATED COPY\n");
    //First call to Blur 3x3
    TimerStart();
    vglClBlurSq3(img, out);
    vglClFlush();
    printf("First call to          Blur 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Blur 3x3
    int p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClBlurSq3(img, out);
    }
    vglClFlush();
    printf("Time spent on %8d Blur 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_blur33.tif");
    cvSaveImage(outFilename, out->ipl);

    // Convolution kernels
    float kernel33[3][3]    = { {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f},
                                {2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f},
                                {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f}, }; //blur 3x3
    float kernel55[5][5]    = { {1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f},
                                {4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f},
                                {6.0f/256.0f, 24.0f/256.0f, 36.0f/256.0f, 24.0f/256.0f, 6.0f/256.0f},
                                {4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f},
                                {1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f}, }; //blur 5x5


    //First call to Convolution 3x3
    TimerStart();
    vglClConvolution(img, out, (float*) kernel33, 3, 3);
    vglClFlush();
    printf("First call to          Convolution 3x3:         %s\n", getTimeElapsedInSeconds());

    //Total time spent on n operations Convolution 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClConvolution(img, out, (float*) kernel33, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d Convolution 3x3:         %s \n", nSteps, getTimeElapsedInSeconds());


    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_conv33.tif");
    cvSaveImage(outFilename, out->ipl);

    //Total time spent on n operations Convolution 5x5
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClConvolution(img, out, (float*) kernel55, 5, 5);
    }
    vglClFlush();
    printf("Time spent on %8d Convolution 5x5:         %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_conv55.tif");
    cvSaveImage(outFilename, out->ipl);

    //First call to Erode 3x3
    float erodeMask[9] = { 0, 1, 0, 1, 1, 1, 0, 1, 0 };
    TimerStart();
    vglClErode(img, out, erodeMask, 3, 3);
    vglClFlush();
    printf("First call to          Erode 3x3:               %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Erode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClErode(img, out, erodeMask, 3, 3);
    }
    vglClFlush();
    printf("Time spent on %8d Erode 3x3:               %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_erosion.tif");
    cvSaveImage(outFilename, out->ipl);

    //First call to Invert
    TimerStart();
    vglClInvert(img,out);
    vglClFlush();
    printf("Fisrt call to          Invert:                  %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Invert
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClInvert(img, out);
    }
    vglClFlush();
    printf("Time spent on %8d Invert:                  %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_invert.tif");
    cvSaveImage(outFilename, out->ipl);

    //First call to Threshold
    TimerStart();
    vglClThreshold(img, out, 0.5);
    vglClFlush();
    printf("Fisrt call to          Threshold:               %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Threshold
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClThreshold(img, out, 0.5);
    }
    vglClFlush();
    printf("Time spent on %8d Threshold:               %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_thresh.tif");
    cvSaveImage(outFilename, out->ipl);

    //First call to Copy GPU->GPU
    TimerStart();
    vglClCopy(img,out);
    vglClFlush();
    printf("First call to          Copy GPU->GPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->GPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglClCopy(img, out);
    }
    vglClFlush();
    printf("Time spent on %8d copy GPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_gpuCopy.tif");
    cvSaveImage(outFilename, out->ipl);

    VglImage* gray = vglCreateImage(img);
    gray->ipl = cvCreateImage(cvGetSize(gray->ipl),IPL_DEPTH_8U,1);

  if (img->nChannels == 1)
  {
    printf("Image is already grayscale. Not running conversion BGR->Gray and BGR->RGBA\n");
  }  
  else
  {
    //First call to Convert BGR->Gray
    TimerStart();
    cvCvtColor(img->ipl, gray->ipl, CV_BGR2GRAY);
    vglClFlush();
    printf("First call to          Convert BGR->Gray (CPU): %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Convert BGR->Gray
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cvCvtColor(img->ipl, gray->ipl, CV_BGR2GRAY);
    }
    vglClFlush();
    printf("Time spent on %8d Convert BGR->Gray (CPU): %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(gray, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_bgrToGrayCpu.tif");
    cvSaveImage(outFilename, gray->ipl);

    //First call to Convert BGR->RGBA
    IplImage* iplBGR  = cvCreateImage(cvGetSize(img->ipl), img->depth, 3);
    IplImage* iplRGBA = cvCreateImage(cvGetSize(img->ipl), img->depth, 4);
    cvCvtColor(img->ipl, iplBGR, CV_RGBA2BGR);

    TimerStart();
    cvCvtColor(iplBGR, iplRGBA, CV_BGR2RGBA);
    vglClFlush();
    printf("First call to          Convert BGR->RGBA (CPU): %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Convert BGR->RGBA
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cvCvtColor(iplBGR, iplRGBA, CV_BGR2RGBA);
    }
    vglClFlush();
    printf("Time spent on %8d Convert BGR->RGBA (CPU): %s\n", nSteps, getTimeElapsedInSeconds());


    sprintf(outFilename, "%s%s", outPath, "/out_cl_bgrToRgbaCpu.tif");
    cvSaveImage(outFilename, iplRGBA);
  }

    //First call to Copy CPU->GPU
    vglCheckContext(img, VGL_RAM_CONTEXT);
    TimerStart();
    vglSetContext(img, VGL_RAM_CONTEXT);
    vglClUpload(img);
    vglClFlush();
    printf("First call to          Copy CPU->GPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy CPU->GPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglSetContext(img, VGL_RAM_CONTEXT);
        vglClUpload(img);
    }
    vglClFlush();
    printf("Time spent on %8d copy CPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(img, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_upload.tif");
    cvSaveImage(outFilename, img->ipl);

    //First call to Copy GPU->CPU
    vglCheckContext(img, VGL_CL_CONTEXT);
    TimerStart();
    vglSetContext(img, VGL_CL_CONTEXT);
    vglClDownload(img);
    vglClFlush();
    printf("First call to          Copy GPU->CPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->CPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        vglSetContext(img, VGL_CL_CONTEXT);
        vglClDownload(img);
    }
    vglClFlush();
    printf("Time spent on %8d copy GPU->CPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(img, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_download.tif");
    cvSaveImage(outFilename, img->ipl);

    //flush
    vglClFlush();
    return 0;

}

#endif
