#include "vglImage.h"
#include "vglClImage.h"
#include "cl2cpp_shaders.h"
#include "vglMMD.h"
#include "vglContext.h"
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <demo/timer.h>
#include <math.h>
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
    //First call to Fuzzy Erode 3x3
    TimerStart();
	vglClFuzzyAlgErosion(img,out,erodeMask,3,3);
    printf("First call to        Alg Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    int p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyAlgErosion(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Alg Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_alg_fuzzy_erode.jpg");
    cvSaveImage(outFilename, out->ipl);

	//vglIpl3To4Channels(out);

	TimerStart();
	vglClFuzzyAlgDilation(img,out,erodeMask,3,3);
    printf("First call to        Alg Fuzzy Dilate 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyAlgDilation(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Alg Fuzzy Dilate 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_alg_fuzzy_dilate.jpg");
    cvSaveImage(outFilename, out->ipl);

	
	TimerStart();
	vglClFuzzyGeoDilation(img,out,erodeMask,3,3);
    printf("First call to        Geo Fuzzy Dilate 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyGeoDilation(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Geo Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_Geo_fuzzy_dilate.jpg");
    cvSaveImage(outFilename, out->ipl);


	
	TimerStart();
	vglClFuzzyGeoErosion(img,out,erodeMask,3,3);
    printf("First call to        Geo Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyGeoErosion(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Geo Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_Geo_fuzzy_erode.jpg");
    cvSaveImage(outFilename, out->ipl);


	TimerStart();
	vglClFuzzyArithDilation(img,out,erodeMask,3,3);
    printf("First call to        Arith Fuzzy Dilate 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyArithDilation(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Arith Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_arith_fuzzy_dilate.jpg");
    cvSaveImage(outFilename, out->ipl);


	
	TimerStart();
	vglClFuzzyArithErosion(img,out,erodeMask,3,3);
    printf("First call to        Arith Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyArithErosion(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Arith Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_arith_fuzzy_erode.jpg");
    cvSaveImage(outFilename, out->ipl);

	
	TimerStart();
	vglClFuzzyBoundDilation(img,out,erodeMask,3,3);
    printf("First call to        Bound Fuzzy Dilate 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyBoundDilation(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Bound Fuzzy Dilate 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_Bound_fuzzy_dilate.jpg");
    cvSaveImage(outFilename, out->ipl);

	
	TimerStart();
	vglClFuzzyBoundErosion(img,out,erodeMask,3,3);
    printf("First call to        Bound Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyBoundErosion(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Bound Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_bound_fuzzy_erode.jpg");
    cvSaveImage(outFilename, out->ipl);

	TimerStart();
	vglClFuzzyDrasticDilation(img,out,erodeMask,3,3);
    printf("First call to        Drastic Fuzzy Dilate 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyDrasticDilation(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Drastic Fuzzy Dilate 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_drastic_fuzzy_dilate.jpg");
    cvSaveImage(outFilename, out->ipl);

	
	TimerStart();
	vglClFuzzyDrasticErosion(img,out,erodeMask,3,3);
    printf("First call to        Drastic Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyDrasticErosion(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Drastic Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_drastic_fuzzy_erode.jpg");
    cvSaveImage(outFilename, out->ipl);
	
	
	TimerStart();
	vglClFuzzyDaPDilation(img,out,erodeMask,3,3);
    printf("First call to        Dubois and Prade Fuzzy Dilate 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyDaPDilation(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Dubois and Prade Fuzzy Dilate 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_DaP_fuzzy_dilate.jpg");
    cvSaveImage(outFilename, out->ipl);

	
	TimerStart();
	vglClFuzzyDaPErosion(img,out,erodeMask,3,3);
    printf("First call to        Dubois and Prade Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyDaPErosion(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Dubois and Prade Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_DaP_fuzzy_erode.jpg");
    cvSaveImage(outFilename, out->ipl);

	
	TimerStart();
	vglClFuzzyHamacherDilation(img,out,erodeMask,3,3);
    printf("First call to        Hamacher Fuzzy Dilate 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyHamacherDilation(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d hamacher Fuzzy Dilate 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_Hamacher_fuzzy_dilate.jpg");
    cvSaveImage(outFilename, out->ipl);

	
	TimerStart();
	vglClFuzzyHamacherErosion(img,out,erodeMask,3,3);
    printf("First call to        Hamacher Fuzzy Erode 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations FuzzyErode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
		vglClFuzzyHamacherErosion(img, out,erodeMask,3,3);
		//vglClCopy(out,img);
    }
    printf("Time spent on %8d Hamacher Fuzzy Erode 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(out, VGL_RAM_CONTEXT);
	//vglIpl4To3Channels(out);
    sprintf(outFilename, "%s%s", outPath, "/out_cl_Hamacher_fuzzy_erode.jpg");
    cvSaveImage(outFilename, out->ipl);

	return 0;
}