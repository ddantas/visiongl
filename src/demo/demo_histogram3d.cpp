#include "vglImage.h"
#include "vglClImage.h"
#include "vglDcmtkIo.h"
#include "vglClFunctions.h"
#include "demo/timer.h"


int main(int argc, char* argv[])
{

	vglInit(50,50);
	vglClInit();

	if (argc < 2)
	{
		printf("Usage: demo_histogram file.dicom\n");
		printf("bad arguments, read usage again\n");
		exit(1);
	}

	char* image_path = argv[1];

	printf("VisionCL on %s\n\n",image_path);

    //REMEMBER, LOAD A GRAY LEVEL IMAGE
    VglImage* img = vglDcmtkLoadDicom(image_path);
    VglImage* out = vglCreate3dImage(cvSize(img->shape[VGL_WIDTH],img->shape[VGL_HEIGHT]),img->depth,img->nChannels,img->shape[VGL_LENGTH]);
	
    //OpenCL histogram
    TimerStart();
    vglClHistogram(img);
    printf("First call to          clHistogram: %s\n", getTimeElapsedInSeconds());
    TimerStart();
    int* histogram = vglClHistogram(img);
    printf("Testing clHistogram time: %s\n", getTimeElapsedInSeconds());

    //CPU histogram
    TimerStart();
    int* histogram_cpu_r = (int*) malloc(256*sizeof(int));

    memset(histogram_cpu_r,0x0,256*sizeof(int));

    unsigned char* pixels = (unsigned char*) img->ndarray;

    for(int i = 0; i < img->shape[VGL_WIDTH]*img->shape[VGL_HEIGHT]*img->shape[VGL_LENGTH];i++)
    {
      histogram_cpu_r[pixels[i]]++;
    }

    printf("Testing Histogram CPU time: %s\n", getTimeElapsedInSeconds());
    //Test if they are equal
    int t = 0;
    for(int i = 0; i < 256; i++)
    {
      t += histogram_cpu_r[i] - histogram[i];
    }
    printf("Total diff %d\n", t);

    vglCl3dHistogramEq(img,out,0);
    vglClDownload(out);

    vglDcmtkSaveDicom(out,"C:/Users/H_DANILO/Dropbox/TCC/TCC_DICOM_THRESH.dcm",0);
    
    return 0;
}