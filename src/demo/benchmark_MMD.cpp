#include "vglImage.h"
#include "vglClImage.h"
#include "cl2cpp_shaders.h"
#include "cl2cpp_MM.h"
#include "vglContext.h"
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <demo/timer.h>
#include <math.h>
#include <fstream>


void benchmark(VglImage* in, VglImage* out, float* mask, int size_x, int size_y, char* output_filename, int nSteps, void (*mmdPtr)(VglImage*,VglImage*,float*,int,int,float))
{
	const float gama = 0.2f;
	TimerStart();
	mmdPtr(in,out,mask,size_x,size_y,gama);
	printf("First call to        Alg Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
	//Total time spent on n operations FuzzyErode 3x3
	int p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		mmdPtr(in, out,mask,size_x,size_y,gama);
		//vglClCopy(out,img);
	}
	printf("Time spent on %8d Alg Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

	vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
	cvSaveImage(output_filename, out->ipl);
}

void benchmark(VglImage* in, VglImage* out, float* mask, int size_x, int size_y, char* output_filename, int nSteps, void (*mmdPtr)(VglImage*,VglImage*,float*,int,int))
{
	TimerStart();
	mmdPtr(in,out,mask,size_x,size_y);
	printf("First call to        Alg Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
	//Total time spent on n operations FuzzyErode 3x3
	int p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		mmdPtr(in, out,mask,size_x,size_y);
		//vglClCopy(out,img);
	}
	printf("Time spent on %8d Alg Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

	vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
	cvSaveImage(output_filename, out->ipl);
}

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
    VglImage* img = vglLoadImage(inFilename, 1, 0);

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
            //vglIpl3To4Channels(img);
        }
    }

    if (img == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s", str.c_str());
    }
    VglImage* out = vglCreateImage(img);
	float erodeMask[9] = { 0, 1, 0, 1, 1, 1, 0, 1, 0 };
	sprintf(outFilename, "%s%s", outPath, "/out_cl_alg_fuzzy_erode.jpg");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyAlgErosion);
    benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyAlgDilation);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyGeoErosion);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyGeoDilation);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyArithErosion);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyArithDilation);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyBoundErosion);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyBoundDilation);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyDrasticErosion);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyDrasticDilation);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyDaPErosion);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyDaPDilation);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyHamacherErosion);
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyHamacherDilation);

	return 0;
}