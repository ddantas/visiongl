

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "demo/timer.h"


int main(int argc, char** argv)
{

  printf("argc = %d\n", argc);
  if (argc < 2){
    fprintf(stderr, "Error: please type as first parameter image name\n");
    exit(255);
  }


        //printf("main: printing CL context before init\n");
        //vglClPrintContext();	

	vglInit(50,50);
	vglClInit();

        //printf("main: printing CL context after init\n");
        //vglClPrintContext();	

	char* image_path = (char*) argv[1];
	VglImage* img = vglLoadImage(image_path,1,0);

  if (img == NULL){
    fprintf(stderr, "Error: unable to open image %s\n", image_path);
    exit(255);
  }
        

        vglCheckContext(img, VGL_CL_CONTEXT);

        if (0)
        {// pipeline test 
          VglImage* img1 = vglCreateImage(img);
          vglClInvert(img, img1);

          VglImage* img2 = vglCreateImage(img);
          //float convolution[3][3] = { {0.0f/9.0f,0.0f/9.0f,0.0f/9.0f},{1.0f/9.0f,1.0f/9.0f,1.0f/9.0f},{0.0f/9.0f,0.0f/9.0f,0.0f/9.0f} }; //Operador blur
          vglClBlurSq3(img1, img2);

          VglImage* img3 = vglCreateImage(img);
          vglClThreshold(img2, img3, .15f);


          vglClCopy(img2, img);
          vglCheckContext(img, VGL_RAM_CONTEXT);
          cvSaveImage("../images/lenaout_pipeline.tif", img->ipl);
	}
	// exit(0);




	/*
        printf("-----calling vglClUpload\n");
        //vglClUpload(img);
        vglCheckContext(img, VGL_CL_CONTEXT);
        printf("-----called vglClUpload\n");
        cvSet(img->ipl, CV_RGB(255,0,0));
        cvSaveImage("../images/lenaout_conv33_red.tif", img->ipl);

	//float kernelMean[3][3] = { {0.0f/9.0f,0.0f/9.0f,0.0f/9.0f},{1.0f/9.0f,1.0f/9.0f,1.0f/9.0f},{0.0f/9.0f,0.0f/9.0f,0.0f/9.0f} }; //Operador blur
	vglClBlurSq3(img, img);


        printf("calling vglClDownload\n");
        //vglClDownload(img);
        //vglSetContext(img, VGL_CL_CONTEXT);
        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_conv33_download.tif", img->ipl);
        //vglClPrintContext();	
	*/

	if (img == NULL)
	{
                printf("Y\n");
		std::string str("cvLoadImage/File not found: ");
                printf("Y\n");
		str.append(image_path);
                printf("Y\n");
		vglClCheckError(-1,(char*) str.c_str());
                printf("Y\n");
	}
	VglImage* out = vglCreateImage(img);
	//float convolution[3][3] = { {1.0f/13.0f,2.0f/13.0f,1.0f/13.0f},{2.0f/13.0f,1.0f/13.0f,2.0f/13.0f},{1.0f/13.0f,2.0f/13.0f,1.0f/13.0f} }; //Operador blur
	long elapsed_us;

	//Primeira chamada a blurSq3
	TimerStart();
	vglClBlurSq3(img, out);
	elapsed_us = TimerElapsed();
	printf("Primeira chamada da vglClBlurSq3: %.6f ms \n", elapsed_us/1000.0);
	//Mede o tempo para 1000 blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClBlurSq3(img, out);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 blur 3x3: %.2f s\n", elapsed_us/1000000.0);


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
	elapsed_us = TimerElapsed();
	printf("Primeira chamada da vglClConvolution: %.6f ms \n", elapsed_us/1000.0);
	//Mede o tempo para 1000 convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClConvolution(img, out, (float*) kernel33, 3, 3);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 convolucoes 3x3: %.2f s\n", elapsed_us/1000000.0);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_conv33.tif", out->ipl);


	//Mede o tempo para 1000 convoluções 5x5 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClConvolution(img, out, (float*) kernel55, 5, 5);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 convolucoes 5x5: %.2f s\n", elapsed_us/1000000.0);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_conv55.tif", out->ipl);

	//Primeira chamada a threshold
	TimerStart();
	vglClThreshold(img,out,0.5f);
	elapsed_us = TimerElapsed();
	printf("Primeira chamada da vglClThreshold: %.6f ms \n", elapsed_us/1000.0);
	//Mede o tempo para 1000 thresholds sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClThreshold(out, out, 0.5f);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 threshold: %.2f s\n", elapsed_us/1000000.0);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_thresh.tif", out->ipl);

	//Primeira chamada a vglClInvert
	TimerStart();
	vglClInvert(img,out);
	elapsed_us = TimerElapsed();
	printf("Primeira chamada da vglClInvert: %.6f ms \n", elapsed_us/1000.0);
	//Mede o tempo para 1000 invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClInvert(out, out);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 invert: %.2f s\n", elapsed_us/1000000.0);
	VglImage* gray = vglCreateImage(img);
	gray->ipl = cvCreateImage(cvGetSize(gray->ipl),IPL_DEPTH_8U,1);

        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_invert.tif", out->ipl);

	//Primeira chamada a vglClCopy
	TimerStart();
	vglClCopy(img,out);
	elapsed_us = TimerElapsed();
	printf("Primeira chamada da vglClCopy: %.6f ms \n", elapsed_us/1000.0);
	//Mede o tempo para 1000 copia GPU->GPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClCopy(img, out);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 copia GPU->GPU: %.2f s\n", elapsed_us/1000000.0);

        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_clcopy.tif", out->ipl);

	//Mede o tempo para 1000 conversão RGB->Grayscale na CPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvCvtColor(img->ipl,gray->ipl, CV_BGR2GRAY);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 conversões BGR->Gray: %.2f s\n", elapsed_us/1000000.0);


        vglCheckContext(gray, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_rgbtogray.tif", gray->ipl);

	//Mede o tempo para 1000 conversão RGB->RGBA na CPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvCvtColor(img->ipl,out->iplRGBA, CV_BGR2RGBA);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 conversões BGR->RGBA: %.2f s\n", elapsed_us/1000000.0);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_rgbtorgba.tif", out->iplRGBA);

	//Mede o tempo para 1000 copia CPU->GPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClUpload(img);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 copia CPU->GPU, inclui conversão BGR->RGBA: %.2f s\n", elapsed_us/1000000.0);


        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_upload.tif", img->ipl);

	//Mede o tempo para 1000 copia GPU->CPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClDownload(img);
	}
	elapsed_us = TimerElapsed();
	printf("Tempo gasto para fazer 1000 copia GPU->CPU inclui conversão RGBA->BGR: %.2f s\n", elapsed_us/1000000.0);


        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_download.tif", img->ipl);



	//flush
	vglClFlush();
	//system("pause");
	return 0;
}
