#define __OPENCL__
#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "demo/timer.h"
#include <fstream>

/*
	argv[1] = input_image_path
	argv[2] = number of operations to execute
*/
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: OpenCVCLBenchmark lena_1024.tiff 1000\n");
		printf("for this example, will run the program for lena_1024.tiff using 1000 operations\n");
		printf("bad arguments, read usage again\n");
		exit(1);
	}
	vglInit(50,50);
	vglClInit();
	int limite = atoi(argv[2]);
	char* image_path = argv[1];
	printf("VisionCL on %s for %d operations\n\n",image_path,limite);
	
	VglImage* img = vglLoadImage(image_path,1,0);

	if (img == NULL)
	{
        std::string str("cvLoadImage/File not found: ");
		str.append(image_path);
		printf("%s",str.c_str());
	}
	VglImage* out = vglCreateImage(img);

	//Primeira chamada a blurSq3
	TimerStart();
	vglClBlurSq3(img, out);
	printf("Primeira chamada da vglClBlurSq3: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" blur 3x3 sem a cria��o da opera��o
	int p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglClBlurSq3(img, out);
	}
	printf("Tempo gasto para fazer %d blur 3x3: %s\n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_blur33.tif", out->ipl);

    // Kernels para convolucao
	float kernel33[3][3]    = { {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f},
                                    {2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f},
                                    {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f}, }; //blur 3x3
	float kernel55[5][5]    = { {1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f},
                                    {4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f},
                                    {6.0f/256.0f, 24.0f/256.0f, 36.0f/256.0f, 24.0f/256.0f, 6.0f/256.0f},
                                    {4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f},
                                    {1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f}, }; //blur 5x5


	//Primeira chamada a vglClConvolution
	TimerStart();
	vglClConvolution(img, out, (float*) kernel33, 3, 3);
	printf("Primeira chamada da vglClConvolution: %s\n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" convolu��es 3x3 sem a cria��o da opera��o
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglClConvolution(img, out, (float*) kernel33, 3, 3);
	}
	printf("Tempo gasto para fazer %d convolucoes 3x3: %s \n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_conv33.tif", out->ipl);

	//Mede o tempo para 1000 convolu��es 5x5 sem a cria��o da opera��o
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglClConvolution(img, out, (float*) kernel55, 5, 5);
	}
	printf("Tempo gasto para fazer %d convolucoes 5x5: %s\n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_conv55.tif", out->ipl);

	//Primeira chamada a Erosion
	float erodemask[9] = { 0, 1, 0, 1, 1, 1, 0, 1, 0 };
	TimerStart();
	vglClErosion(img,out,erodemask,3,3);
	printf("Primeira chamada da vglClErosion: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" erosions sem a cria��o da opera��o
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglClErosion(img,out,erodemask,3,3);
	}
	printf("Tempo gasto para fazer %d erosions: %s\n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_erosion.tif", out->ipl);

	//Primeira chamada a vglClInvert
	TimerStart();
	vglClInvert(img,out);
	printf("Primeira chamada da vglClInvert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" invert sem a cria��o da opera��o
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglClInvert(img, out);
	}
	printf("Tempo gasto para fazer %d invert: %s\n",limite, getTimeElapsedInSeconds());
	VglImage* gray = vglCreateImage(img);
	gray->ipl = cvCreateImage(cvGetSize(gray->ipl),IPL_DEPTH_8U,1);

        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_invert.tif", out->ipl);

	//Primeira chamada a vglClCopy
	TimerStart();
	vglClCopy(img,out);
	printf("Primeira chamada da vglClCopy: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" copias GPU->GPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglClCopy(img, out);
	}
	printf("Tempo gasto para fazer %d copia GPU->GPU: %s\n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_clcopy.tif", out->ipl);

	//Mede o tempo para "limite" convers�o RGB->Grayscale na CPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cvCvtColor(img->ipl,gray->ipl, CV_BGR2GRAY);
	}
	printf("Tempo gasto para fazer %d convers�es BGR->Gray: %s\n", limite, getTimeElapsedInSeconds());

        vglCheckContext(gray, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_rgbtogray.tif", gray->ipl);

	//Mede o tempo para limite convers�o RGB->RGBA na CPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cvCvtColor(img->ipl,out->iplRGBA, CV_BGR2RGBA);
	}
	printf("Tempo gasto para fazer %d convers�es BGR->RGBA: %s\n", limite, getTimeElapsedInSeconds());


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_rgbtorgba.tif", out->iplRGBA);

	//Mede o tempo para "limite" copia CPU->GPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglClUpload(img);
	}
	printf("Tempo gasto para fazer %d copia CPU->GPU: %s\n", limite, getTimeElapsedInSeconds());

        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_upload.tif", img->ipl);

	//Mede o tempo para "limite" copia GPU->CPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglClDownload(img);
	}
	printf("Tempo gasto para fazer %d copia GPU->CPU: %s\n", limite, getTimeElapsedInSeconds());

        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_download.tif", img->ipl);

	//flush
	vglClFlush();
	return 0;
}
