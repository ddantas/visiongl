//#define cimg_medcon_path "C:/Program Files/XMedCon/bin/medcon.bat"
//#define cimg_imagemagick_path "C:/Program Files/ImageMagick-6.8.9-Q16/convert.exe"
#include <CImg.h>
#include <fstream>
#include "timer.h"

using namespace cimg_library;

int main(int argc, char* argv[])
{
	cimg::medcon_path("C:/Program Files/XMedCon/bin/medcon.exe", true);
	cimg::imagemagick_path("C:/Program Files/ImageMagick-6.8.9-Q16/convert.exe", true);

	if (argc != 3)
	{
		printf("Usage: OpenCVCLBenchmark lena_1024.tiff 1000\n");
		printf("for this example, will run the program for lena_1024.tiff using 1000 operations\n");
		printf("bad arguments, read usage again\n");
		exit(1);
	}

	int limite = atoi(argv[2]);
	char* image_path = argv[1];
	printf("VisionCL on %s for %d operations\n\n",image_path,limite);
	
	CImg<> img(image_path);
	/*if (img == NULL)
	{
        std::string str("cvLoadImage/File not found: ");
		str.append(image_path);
		printf("%s",str.c_str());
	}*/
	
	CImg<> out(img);
	TimerStart();
	img.blur_median(2);
	printf("Primeira chamada da vglClBlurSq3: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.blur_median(2);
	}
	printf("Tempo gasto para fazer %d blur 3x3: %s\n",limite, getTimeElapsedInSeconds());


	// Kernels para convolucao
	//float* mask333 = (float*) malloc(sizeof(float)*27);
	CImg<float> mask333 = CImg<>(3,3,3).fill(1.0f/27.0f,true);
	CImg<float> mask555 = CImg<>(5,5,5).fill(1.0f/125.0f,true);
	
	//5x5x5 mask for convolution
	//float* mask555 = (float*) malloc(sizeof(float)*125);
	
	//Primeira chamada a vglClConvolution
	TimerStart();
	img.convolve(mask333);
	printf("Primeira chamada da vglClConvolution: %s\n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.convolve(mask333);
	}
	printf("Tempo gasto para fazer %d convolucoes 3x3: %s \n",limite, getTimeElapsedInSeconds());
	
	//Mede o tempo para 1000 convoluções 5x5 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.convolve(mask555);
	}
	printf("Tempo gasto para fazer %d convolucoes 5x5: %s\n",limite, getTimeElapsedInSeconds());

	//Primeira chamada a Erosion
	CImg<float> erosion333 = CImg<>(3,3,3).fill(1.0f,true);
	TimerStart();
	img.erode(erosion333);
	printf("Primeira chamada da vglClErosion: %s \n", getTimeElapsedInSeconds());
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.erode(erosion333);
	}
	printf("Tempo gasto para fazer %d erosions: %s\n",limite, getTimeElapsedInSeconds());

	//Primeira chamada a vglClInvert
	TimerStart();
	img.normalize(img.max(),img.min());
	printf("Primeira chamada da vglClInvert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.normalize(img.max(),img.min());
	}
	printf("Tempo gasto para fazer %d invert: %s\n",limite, getTimeElapsedInSeconds());

	TimerStart();
	img.threshold(127);
	printf("Primeira chamada da threshold: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.threshold(127);
	}
	printf("Tempo gasto para fazer %d threshold: %s\n",limite, getTimeElapsedInSeconds());

	TimerStart();
	out = CImg<>(img);
	printf("Primeira chamada da copia: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		out = CImg<>(img);
	}
	printf("Tempo gasto para fazer %d copia: %s\n",limite, getTimeElapsedInSeconds());

}
