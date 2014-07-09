
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

    if (argc != 4)
    {
        printf("\nUsage: demo_benchmark_cl3d lena_1024.tiff 1000 /tmp\n\n");
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

    printf("VisionGL-OpenCL3D on %s, %d operations\n\n", inFilename, nSteps);
       
    printf("CREATING IMAGE\n");
    VglImage* img = vglCreateImage(inFilename, 0, 16);
    
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

    printf("CHECKING if img is null\n");
    if (img == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s", str.c_str());
    }
    vglPrintImageInfo(img);

    VglImage* out = vglCreate3dImage(cvSize(img->shape[VGL_WIDTH],img->shape[VGL_HEIGHT]), img->depth, 4, 17, 0);
    vglPrintImageInfo(out);

	//Primeira chamada a blurSq3
	TimerStart();
	vglCl3dBlurSq3(img, out);
	printf("Primeira chamada da vglClBlurSq3: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "nSteps" blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglCl3dBlurSq3(img, out);
	}
	printf("Tempo gasto para fazer %d blur 3x3: %s\n",nSteps, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_blur333.%d.jpg", 0,16);

    // Kernels para convolucao
	float* mask333 = (float*) malloc(sizeof(float)*27);

	for(int i = 0; i < 27; i++)
		mask333[i] = 1.0f/27.0f;
	//5x5x5 mask for convolution
	float* mask555 = (float*) malloc(sizeof(float)*125);
	for(int i = 0; i < 125; i++)
		mask555[i] = 1.0f/125.0f;


	//Primeira chamada a vglClConvolution
	TimerStart();
	vglCl3dConvolution(img, out, (float*) mask333, 3, 3, 3);
	printf("Primeira chamada da vglClConvolution: %s\n", getTimeElapsedInSeconds());
	//Mede o tempo para "nSteps" convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglCl3dConvolution(img, out, (float*) mask333, 3, 3, 3);
	}
	printf("Tempo gasto para fazer %d convolucoes 3x3: %s \n",nSteps, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_conv333.%d.jpg", 0,16);

	//Mede o tempo para 1000 convoluções 5x5 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglCl3dConvolution(img, out, (float*) mask555, 5, 5, 5);
	}
	printf("Tempo gasto para fazer %d convolucoes 5x5: %s\n",nSteps, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_conv555.%d.jpg", 0,16);

	//Primeira chamada a Erosion
	float erosion[] = { 1, 1, 1, 1, 1, 1, 1, 1 , 1,
					  1, 1, 1, 1, 1, 1, 1, 1 , 1,
					  1, 1, 1, 1, 1, 1, 1, 1 , 1};
	TimerStart();
	vglCl3dErosion(img,out,erosion,3,3,3);
	printf("Primeira chamada da vglClErosion: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "nSteps" erosions sem a criação da operação
	p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglCl3dErosion(img,out,erosion,3,3,3);
	}
	printf("Tempo gasto para fazer %d erosions: %s\n",nSteps, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_erod333.%d.jpg", 0,16);

	//Primeira chamada a vglClInvert
	TimerStart();
	vglCl3dNot(img,out);
	printf("Primeira chamada da vglClInvert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "nSteps" invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglCl3dNot(img, out);
	}
	printf("Tempo gasto para fazer %d invert: %s\n",nSteps, getTimeElapsedInSeconds());
	
        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_invert.%d.jpg", 0,16);

	//Primeira chamada a vglCl3dThreshold
	TimerStart();
	vglCl3dThreshold(img, out, 127.0);
	printf("Primeira chamada da vglCl3dThreshold: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "nSteps" invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglCl3dThreshold(img, out, 127.0);
	}
	printf("Tempo gasto para fazer %d threshold: %s\n",nSteps, getTimeElapsedInSeconds());
	
        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_threshold.%d.jpg", 0,16);

	//Primeira chamada a vglClCopy
	TimerStart();
	vglCl3dCopy(img,out);
	printf("Primeira chamada da vglClCopy: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "nSteps" copias GPU->GPU
	p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglCl3dCopy(img, out);
	}
	printf("Tempo gasto para fazer %d copia GPU->GPU: %s\n",nSteps, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_copy.%d.jpg", 0,16);

	
	//Mede o tempo para "nSteps" copia CPU->GPU
	p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglClUpload(img);
	}
	printf("Tempo gasto para fazer %d copia CPU->GPU: %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(img, VGL_RAM_CONTEXT);

	//Mede o tempo para "nSteps" copia GPU->CPU
	p = 0;
	TimerStart();
	while (p < nSteps)
	{
		p++;
		vglClDownload(img);
	}
	printf("Tempo gasto para fazer %d copia GPU->CPU: %s\n", nSteps, getTimeElapsedInSeconds());

    vglCheckContext(img, VGL_RAM_CONTEXT);

	//flush
	vglClFlush();
	return 0;
}

#endif
