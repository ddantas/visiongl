#include "vglImage.h"
#include "vglClImage.h"
#include "vglDcmtkIo.h"
#include "vglClFunctions.h"
#include "demo/timer.h"
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>


int main(int argc, char* argv[])
{

	vglInit(50,50);
	vglClInit();

	if (argc < 2)
	{
		printf("Usage: demo_histogram file.ext\n");
        printf("File must have 1 channel\n");
		printf("bad arguments, read usage again\n");
		exit(1);
	}

	char* image_path = argv[1];

	printf("VisionCL on %s\n\n",image_path);

    //load 2d gray image
    VglImage* img = vglLoadImage(image_path,0);
	VglImage* out = vglCreateImage(img);
    
    int* eq = (int*) malloc(256*sizeof(int));
    for(int i = 0; i < 256; i++)
    {
      if (i < 96)
        eq[i] = 0;
      else if (i < 200)
        eq[i] = floor((i/256.0f)*img->shape[VGL_WIDTH]*img->shape[VGL_HEIGHT]);
      else
        eq[i] = 0;
    }

    TimerStart();
    vglClHistogram(img);
    printf("First call to          clHistogram: %s\n", getTimeElapsedInSeconds());

    TimerStart();
    vglClHistogramEq(img,out);
    printf("First call to        clHistogramEq: %s\n", getTimeElapsedInSeconds());

    TimerStart();
    vglClGrayLevelTransform(img,out,eq);
    printf("First call to ClGrayLevelTransform: %s\n", getTimeElapsedInSeconds());

    TimerStart();
    int* histogram = vglClHistogram(img);
    printf("Testing clHistogram   time: %s\n", getTimeElapsedInSeconds());
    

    //CPU histogram
    TimerStart();
    int* histogram_cpu = (int*) malloc(256*sizeof(int));

    memset(histogram_cpu,0x0,256*sizeof(int));

    unsigned char* pixels = (unsigned char*) img->ipl->imageData;

    for(int i = 0; i < img->shape[VGL_WIDTH]*img->shape[VGL_HEIGHT];i++)
    {
      histogram_cpu[pixels[i]]++;
    }
    printf("Testing Histogram CPU time: %s\n", getTimeElapsedInSeconds());
    //Test if they are equal
    int t = 0;
    for(int i = 0; i < 256; i++)
    {
      //printf("%d: %d - %d = %d\n",i,histogram_cpu[i],histogram[i],histogram_cpu[i]-histogram[i]);
      t += histogram_cpu[i]-histogram[i];
    }
    printf("Total diff %d\n", t);

    
    TimerStart();
    vglClHistogramEq(img,out);
    printf("Testing Histogram GPU EQ time: %s\n", getTimeElapsedInSeconds());

    TimerStart();
    cvEqualizeHist(img->ipl,out->ipl);
    printf("Testing Histogram CPU EQ time: %s\n", getTimeElapsedInSeconds());

    vglClDownload(out);
    cvSaveImage("C:/Users/H_DANILO/Dropbox/TCC/teste/cl_histeq2dgray.tif",out->ipl);

    TimerStart();
    vglClGrayLevelTransform(img,out,eq);
    printf("Testing GrayLevelTransform time: %s\n", getTimeElapsedInSeconds());

    vglClDownload(out);
    cvSaveImage("C:/Users/H_DANILO/Dropbox/TCC/teste/cl_grayleveltransform2dgray.tif",out->ipl);
    return 0;
}