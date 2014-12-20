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
    //vglIpl3To4Channels(out);
	TimerStart();
	mmdPtr(in,out,mask,size_x,size_y,gama);
        vglClFlush();
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
    vglClFlush();
	printf("Time spent on %8d Alg Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

	vglClDownload(out);
	//vglIpl4To3Channels(out);
	cvSaveImage(output_filename, out->ipl);
}

void benchmark(VglImage* in, VglImage* out, float* mask, int size_x, int size_y, char* output_filename, int nSteps, void (*mmdPtr)(VglImage*,VglImage*,float*,int,int))
{
    //vglIpl3To4Channels(out);
	TimerStart();
	mmdPtr(in,out,mask,size_x,size_y);
        vglClFlush();
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
    vglClFlush();
	printf("Time spent on %8d Alg Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

	vglClDownload(out);
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
	
    VglImage* img = vglLoadImage(inFilename, 1, 0);
    vglPrintImageInfo(img);

    if (img->nChannels == 3)
    {
        vglImage3To4Channels(img);
    }

    if (img == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s", str.c_str());
    }

    VglImage* out = vglCreateImage(img);
	float erodeMask[9] = { 0,1, 0, 1, 1, 1, 0, 1, 0 };
	sprintf(outFilename, "%s%s", outPath, "/out_cl_alg_fuzzy_erode.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyAlgErode);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_alg_fuzzy_dilate.tif");
    benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyAlgDilate);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_geo_fuzzy_erode.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyGeoErode);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_geo_fuzzy_dilate.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyGeoDilate);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_arith_fuzzy_erode.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyArithErode);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_arith_fuzzy_dilate.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyArithDilate);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_bound_fuzzy_erode.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyBoundErode);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_bound_fuzzy_dilate.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyBoundDilate);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_drastic_fuzzy_erode.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyDrasticErode);
	sprintf(outFilename, "%s%s", outPath, "/out_cl_drastic_fuzzy_dilate.tif");
    benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyDrasticDilate);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_DaP_fuzzy_erode.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyDaPErode);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_DaP_fuzzy_dilate.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyDaPDilate);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_Hamacher_fuzzy_erode.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyHamacherErode);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_Hamacher_fuzzy_dilate.tif");
	benchmark(img,out,erodeMask,3,3,outFilename,nSteps,&vglClFuzzyHamacherDilate);

	return 0;
}
