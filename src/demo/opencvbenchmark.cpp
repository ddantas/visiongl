#define __OPENCL__
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "demo/timer.h"
#include <ctime>

#include <fstream>

//#include "demo/timer.h"


int main()
{
	char* image_path = (char*) "../images/lena_1024.tif";
	IplImage* img = cvLoadImage(image_path,1);

	if (img == NULL)
	{
                printf("Y\n");
		std::string str("cvLoadImage/File not found: ");
                printf("Y\n");
		str.append(image_path);
                printf("Y\n");
		printf("%s",str.c_str());
                printf("Y\n");
	}
	IplImage* out = cvCreateImage(cvGetSize(img),img->depth,img->nChannels);

	//Primeira chamada a blurSq3
	TimerStart();
	cvSmooth(img,out,CV_GAUSSIAN,3);
	printf("Primeira chamada da cvSmooth: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvSmooth(img,out,CV_GAUSSIAN,3);
	}
	printf("Tempo gasto para fazer 1000 blur 3x3: %s\n", getTimeElapsedInSeconds());


        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_blur33.tif", out);

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
	
	CvMat* cvkernel33;
	cvkernel33 = cvCreateMat(3,3,CV_32F);

	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			cvSet2D(cvkernel33,i,j,cvScalar(kernel33[i][j]));

	CvMat* cvkernel55;
	cvkernel55 = cvCreateMat(5,5,CV_32F);
	
	for(int i = 0; i < 5; i++)
		for(int j = 0; j < 5; j++)
			cvSet2D(cvkernel55,i,j,cvScalar(kernel55[i][j]));
	
	TimerStart();
	cvFilter2D(img,out, cvkernel33);
	printf("Primeira chamada da Filter2D com kernel 3x3: %s\n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvFilter2D(img,out, cvkernel33);
	}
	printf("Tempo gasto para fazer 1000 convolucoes 3x3: %s \n", getTimeElapsedInSeconds());


        cvSaveImage("../images/lenaout_conv33.tif", out);

	//Mede o tempo para 1000 convoluções 5x5 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvFilter2D(img, out, cvkernel55);
	}
	printf("Tempo gasto para fazer 1000 convolucoes 5x5: %s\n", getTimeElapsedInSeconds());


        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_conv55.tif", out);

	
	//Primeira chamada a threshold
	TimerStart();
	cvThreshold(img,out,0.5f,255,CV_THRESH_BINARY);
	printf("Primeira chamada da cvThreshold: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 thresholds sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvThreshold(img,out,0.5f,255,CV_THRESH_BINARY);
	}
	printf("Tempo gasto para fazer 1000 threshold: %s\n", getTimeElapsedInSeconds());


        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_thresh.tif", out);
	
	
	//Primeira chamada a vglClInvert
	TimerStart();
	
	/*for(int i = 0; i < img->width; i++)
			for(int j = 0; j < img->height; j++)
				cvSet(out,cvScalar(255-cvGet2D(img,i,j).val[0],255-cvGet2D(img,i,j).val[1],255-cvGet2D(img,i,j).val[2]));*/
	CvMat* submat = cvCreateMat(img->width,img->height,CV_8UC3);
	for(int i = 0; i < img->width; i++)
		for(int j = 0; j < img->height; j++)
			cvSet2D(submat,i,j,CV_RGB(255,255,255));
	
	cvSub(submat,img,out);
	printf("Primeira chamada da Invert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvSub(submat,img,out);
	}
	printf("Tempo gasto para fazer 1000 Invert: %s\n", getTimeElapsedInSeconds());

	IplImage* gray = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	IplImage* RGBA = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,4);
        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_invert.tif", out);

	//Primeira chamada a vglClCopy
	TimerStart();
	cvCopy(img,out);
	printf("Primeira chamada da cvCopy: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 copia GPU->GPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvCopy(img, out);
	}
	printf("Tempo gasto para fazer 1000 copia CPU->CPU: %s\n", getTimeElapsedInSeconds());

        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_clcopy.tif", out);


	
	//Mede o tempo para 1000 conversão RGB->Grayscale na CPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvCvtColor(img,gray, CV_BGR2GRAY);
	}
	printf("Tempo gasto para fazer 1000 conversões BGR->Gray: %s\n", getTimeElapsedInSeconds());

	
        //vglCheckContext(gray, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_rgbtogray.tif", gray);

	//Mede o tempo para 1000 conversão RGB->RGBA na CPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvCvtColor(img,RGBA, CV_BGR2RGBA);
	}
	printf("Tempo gasto para fazer 1000 conversões BGR->RGBA: %s\n", getTimeElapsedInSeconds());

	
        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_rgbtorgba.tif", RGBA);

	system("pause");
	exit(0);
	
	/*
	//Mede o tempo para 1000 copia CPU->GPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		vglClUpload(img);
	}
	printf("Tempo gasto para fazer 1000 copia CPU->GPU, inclui conversão BGR->RGBA: %s\n", getTimeElapsed());


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
	printf("Tempo gasto para fazer 1000 copia GPU->CPU inclui conversão RGBA->BGR: %s\n", getTimeElapsed());


        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lenaout_download.tif", img->ipl);



	//flush
	vglClFlush();*/
	system("pause");
	return 0;
}
