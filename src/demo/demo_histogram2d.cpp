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
    //OpenCL histogram
    TimerStart();
    vglClHistogram(img);
    printf("First call to          clHistogram: %s\n", getTimeElapsedInSeconds());
    TimerStart();
    int* histogram = vglClHistogram(img);
    printf("Testing clHistogram time: %s\n", getTimeElapsedInSeconds());
    

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
      t += histogram_cpu[i]-histogram[i];
    }
    printf("Total diff %d\n", t);

    vglClHistogramEq(img,out,1);
    vglClDownload(out);
    
    cvSaveImage("C:/Users/H_DANILO/Dropbox/TCC/teste/cl_eq2dgray.tif",out->ipl);
    return 0;
}