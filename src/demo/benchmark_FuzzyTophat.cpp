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


void vglClTopHat(VglImage* src, VglImage* dst, VglImage* buf, float* strel, int strel_size_x, int strel_size_y, void (*mmdErode)(VglImage*,VglImage*,float*,int,int),void (*mmdDilate)(VglImage*,VglImage*,float*,int,int))
{

  mmdErode(src, dst, strel, strel_size_x, strel_size_y);
  mmdDilate(dst, buf, strel, strel_size_x, strel_size_y);

  vglClSub(src, buf, dst);
}

void vglClTopHat(VglImage* src, VglImage* dst, VglImage* buf, float* strel, int strel_size_x, int strel_size_y, void (*mmdErode)(VglImage*,VglImage*,float*,int,int, float),void (*mmdDilate)(VglImage*,VglImage*,float*,int,int, float), float gama)
{

  mmdErode(src, dst, strel, strel_size_x, strel_size_y, gama);
  mmdDilate(dst, buf, strel, strel_size_x, strel_size_y, gama);

  vglClSub(src, buf, dst);
}


void benchmark(VglImage* in, VglImage* out, float* mask, int size_x, int size_y, char* output_filename, int nSteps, void (*mmdErode)(VglImage*,VglImage*,float*,int,int,float),void (*mmdDilate)(VglImage*,VglImage*,float*,int,int,float))
{
	const float gama = 0.2f;
        //vglIpl3To4Channels(out);
        VglImage* buf = vglCreateImage(in);
	TimerStart();
	//mmdPtr(in,out,mask,size_x,size_y,gama);
        vglClTopHat(in, out, buf, mask, size_x, size_y, mmdErode, mmdDilate, gama);
        vglClFlush();
	printf("First call to        Alg Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
	//Total time spent on n operations FuzzyErode 3x3
	int p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		//mmdPtr(in, out,mask,size_x,size_y,gama);
                vglClTopHat(in, out, buf, mask, size_x, size_y, mmdErode, mmdDilate, gama);
		//vglClCopy(out,img);
	}
    vglClFlush();
	printf("Time spent on %8d Alg Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

	vglClDownload(out);
	//vglIpl4To3Channels(out);
	cvSaveImage(output_filename, out->ipl);
        vglReleaseImage(&buf);
}

void benchmark(VglImage* in, VglImage* out, float* mask, int size_x, int size_y, char* output_filename, int nSteps, void (*mmdErode)(VglImage*,VglImage*,float*,int,int),void (*mmdDilate)(VglImage*,VglImage*,float*,int,int))
{
    //vglIpl3To4Channels(out);
        VglImage* buf = vglCreateImage(in);
	TimerStart();
	//mmdPtr(in,out,mask,size_x,size_y);
        vglClTopHat(in, out, buf, mask, size_x, size_y, mmdErode, mmdDilate);
        vglClFlush();
	printf("First call to        Alg Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
	//Total time spent on n operations FuzzyErode 3x3
	int p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		//mmdPtr(in, out,mask,size_x,size_y);
                vglClTopHat(in, out, buf, mask, size_x, size_y, mmdErode, mmdDilate);
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
    iplPrintImageInfo(img->ipl);

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
    int w = 5;
    int h = 5;
    //float erodeMask[] = { 0, 1, 0, 1, 1, 1, 0, 1, 0 };
    float erodeMask[] = {    0,    0,  .125,    0,    0, 
                             0,    0,  .500,    0,    0,
                          .125, .500, 1.000, .500, .125,
                             0,    0,  .500,    0,    0,
                             0,    0,  .125,    0,    0, };
    /*float erodeMask[] = {    .125,  .500, .125, 
                             .500, 1.000, .500, 
                             .125,  .500, .125,  };*/
                             

    sprintf(outFilename, "%s%s", outPath, "/out_cl_std_tophat.tif");
    benchmark(img,out,erodeMask,w,h,outFilename,nSteps,&vglClErode, &vglClDilate);

    sprintf(outFilename, "%s%s", outPath, "/out_cl_alg_fuzzy_tophat.tif");
    benchmark(img,out,erodeMask,w,h,outFilename,nSteps,&vglClFuzzyAlgErode,&vglClFuzzyAlgDilate);

    sprintf(outFilename, "%s%s", outPath, "/out_cl_geo_fuzzy_tophat.tif");
    benchmark(img,out,erodeMask,w,h,outFilename,nSteps,&vglClFuzzyGeoErode,&vglClFuzzyGeoDilate);

    sprintf(outFilename, "%s%s", outPath, "/out_cl_arith_fuzzy_tophat.tif");
    benchmark(img,out,erodeMask,w,h,outFilename,nSteps,&vglClFuzzyArithErode,&vglClFuzzyArithDilate);

    sprintf(outFilename, "%s%s", outPath, "/out_cl_bound_fuzzy_tophat.tif");
    benchmark(img,out,erodeMask,w,h,outFilename,nSteps,&vglClFuzzyBoundErode,&vglClFuzzyBoundDilate);

    sprintf(outFilename, "%s%s", outPath, "/out_cl_drastic_fuzzy_tophat.tif");
    benchmark(img,out,erodeMask,w,h,outFilename,nSteps,&vglClFuzzyDrasticErode,&vglClFuzzyDrasticDilate);

    sprintf(outFilename, "%s%s", outPath, "/out_cl_DaP_fuzzy_tophat.tif");
    benchmark(img,out,erodeMask,w,h,outFilename,nSteps,&vglClFuzzyDaPErode,&vglClFuzzyDaPDilate);

    sprintf(outFilename, "%s%s", outPath, "/out_cl_Hamacher_fuzzy_tophat.tif");
    benchmark(img,out,erodeMask,w,h,outFilename,nSteps,&vglClFuzzyHamacherErode,&vglClFuzzyHamacherDilate);

    return 0;
}
