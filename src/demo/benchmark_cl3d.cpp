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
	
	VglImage* img = vglCreateImage(image_path,0,16,false);

	if (img == NULL)
	{
        std::string str("cvLoadImage/File not found: ");
		str.append(image_path);
		printf("%s",str.c_str());
	}
	VglImage* out = vglCreate3dImage(cvSize(img->ipl->width,img->ipl->height),8,3,17,0);

	//Primeira chamada a blurSq3
	TimerStart();
	vglCl3dBlurSq3(img, out);
	printf("Primeira chamada da vglClBlurSq3: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglCl3dBlurSq3(img, out);
	}
	printf("Tempo gasto para fazer %d blur 3x3: %s\n",limite, getTimeElapsedInSeconds());

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
	//Mede o tempo para "limite" convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglCl3dConvolution(img, out, (float*) mask333, 3, 3, 3);
	}
	printf("Tempo gasto para fazer %d convolucoes 3x3: %s \n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_conv333.%d.jpg", 0,16);

	//Mede o tempo para 1000 convoluções 5x5 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglCl3dConvolution(img, out, (float*) mask555, 5, 5, 5);
	}
	printf("Tempo gasto para fazer %d convolucoes 5x5: %s\n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_conv555.%d.jpg", 0,16);

	//Primeira chamada a Erosion
	float erosion[] = { 1, 1, 1, 1, 1, 1, 1, 1 , 1,
					  1, 1, 1, 1, 1, 1, 1, 1 , 1,
					  1, 1, 1, 1, 1, 1, 1, 1 , 1};
	TimerStart();
	vglCl3dErosion(img,out,erosion,3,3,3);
	printf("Primeira chamada da vglClErosion: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" erosions sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglCl3dErosion(img,out,erosion,3,3,3);
	}
	printf("Tempo gasto para fazer %d erosions: %s\n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_erod333.%d.jpg", 0,16);

	//Primeira chamada a vglClInvert
	TimerStart();
	vglCl3dNot(img,out);
	printf("Primeira chamada da vglClInvert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglCl3dNot(img, out);
	}
	printf("Tempo gasto para fazer %d invert: %s\n",limite, getTimeElapsedInSeconds());
	
        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_invert.%d.jpg", 0,16);

	//Primeira chamada a vglCl3dThreshold
	TimerStart();
	vglCl3dThreshold(img, out, 127.0);
	printf("Primeira chamada da vglCl3dThreshold: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglCl3dThreshold(img, out, 127.0);
	}
	printf("Tempo gasto para fazer %d threshold: %s\n",limite, getTimeElapsedInSeconds());
	
        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_threshold.%d.jpg", 0,16);

	//Primeira chamada a vglClCopy
	TimerStart();
	vglCl3dCopy(img,out);
	printf("Primeira chamada da vglClCopy: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" copias GPU->GPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		vglCl3dCopy(img, out);
	}
	printf("Tempo gasto para fazer %d copia GPU->GPU: %s\n",limite, getTimeElapsedInSeconds());

        vglCheckContext(out, VGL_RAM_CONTEXT);
        vglSaveImage(out,"../images/output/lenaout_copy.%d.jpg", 0,16);

	
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

	//flush
	vglClFlush();
	return 0;
}
