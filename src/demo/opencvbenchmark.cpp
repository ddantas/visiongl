#define __OPENCL__
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "demo/timer.h"
#include <ctime>

#include <fstream>

using namespace cv;


int main()
{
	char* image_path = (char*) "../images/lena_1024.tif";
	cv::Mat img = imread(image_path,1);
	
	vector<int> saveparams;
	saveparams.push_back(CV_IMWRITE_PNG_COMPRESSION);
	saveparams.push_back(1);
	
	if (img.data == NULL)
	{
                printf("Y\n");
		std::string str("cvLoadImage/File not found: ");
                printf("Y\n");
		str.append(image_path);
                printf("Y\n");
		printf("%s",str.c_str());
                printf("Y\n");
	}
	cv::Mat out(img.cols,img.rows,CV_8UC3);

	//Primeira chamada a blurSq3
	TimerStart();
	cv::GaussianBlur(img,out,Size(3,3),0);
	printf("Primeira chamada da Blur: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cv::GaussianBlur(img,out,Size(3,3),0);
	}
	printf("Tempo gasto para fazer 1000 Blur 3x3: %s\n", getTimeElapsedInSeconds());


        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cv::imwrite("../images/lenaout_blur33.png", out,saveparams);

    // Kernels para convolucao
	
	Mat cvkernel33 = (Mat_<float>(3,3) << 1/9.0, 1/9.0, 1/9.0, 
                                          1/9.0, 1/9.0, 1/9.0, 
	                                      1/9.0, 1/9.0, 1/9.0  );

    Mat cvkernel55 = (Mat_<float>(5,5) << 1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                          1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                          1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                          1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                          1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0  );
	
	
	TimerStart();
	cv::filter2D(img,out,-1, cvkernel33);
	printf("Primeira chamada da Filter2D com kernel 3x3: %s\n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cv::filter2D(img,out,-1, cvkernel33);
	}
	printf("Tempo gasto para fazer 1000 convolucoes 3x3: %s \n", getTimeElapsedInSeconds());


        cv::imwrite("../images/lenaout_conv33.png", out,saveparams);

	//Mede o tempo para 1000 convoluções 5x5 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cv::filter2D(img,out,-1, cvkernel55);
	}
	printf("Tempo gasto para fazer 1000 convolucoes 5x5: %s\n", getTimeElapsedInSeconds());


        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cv::imwrite("../images/lenaout_conv55.png", out,saveparams);

	
	//Primeira chamada a threshold
	TimerStart();
	Mat cverode33 = getStructuringElement(MORPH_CROSS, Size( 3, 3 ));
	cv::erode(img,out,cverode33);
	printf("Primeira chamada da cvThreshold: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 thresholds sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cv::erode(img,out,cverode33);
	}
	printf("Tempo gasto para fazer 1000 threshold: %s\n", getTimeElapsedInSeconds());


        //vglCheckContext(out, VGL_RAM_CONTEXT);
        cv::imwrite("../images/lenaout_erode33.png", out,saveparams);
	
	
	//Primeira chamada a vglClInvert
	TimerStart();
	
	/*for(int i = 0; i < img->width; i++)
			for(int j = 0; j < img->height; j++)
				cvSet(out,cvScalar(255-cvGet2D(img,i,j).val[0],255-cvGet2D(img,i,j).val[1],255-cvGet2D(img,i,j).val[2]));*/
	cv::bitwise_not(img,out);
	printf("Primeira chamada da Invert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cv::bitwise_not(img,out);
	}
	printf("Tempo gasto para fazer 1000 Invert: %s\n", getTimeElapsedInSeconds());

	    //vglCheckContext(out, VGL_RAM_CONTEXT);
		cv::imwrite("../images/lenaout_invert33.png", out,saveparams);

	//Primeira chamada a vglClCopy
	TimerStart();
	img.copyTo(out);
	printf("Primeira chamada da cvCopy: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 copia GPU->GPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		img.copyTo(out);
	}
	printf("Tempo gasto para fazer 1000 copia CPU->CPU: %s\n", getTimeElapsedInSeconds());

        //vglCheckContext(out, VGL_RAM_CONTEXT);
		cv::imwrite("../images/lenaout_copy.png", out,saveparams);

	cv::Mat gray(img.cols,img.rows,CV_8UC1);	
	//Mede o tempo para 1000 conversão RGB->Grayscale na CPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvtColor(img,gray, CV_BGR2GRAY);
	}
	printf("Tempo gasto para fazer 1000 conversões BGR->Gray: %s\n", getTimeElapsedInSeconds());

	
        //vglCheckContext(gray, VGL_RAM_CONTEXT);
		cv::imwrite("../images/lenaout_rgbtogray.png", gray,saveparams);

	cv::Mat RGBA(img.cols,img.rows,CV_8UC4);	
	//Mede o tempo para 1000 conversão RGB->RGBA na CPU
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		cvtColor(img,RGBA, CV_BGR2RGBA);
	}
	printf("Tempo gasto para fazer 1000 conversões BGR->RGBA: %s\n", getTimeElapsedInSeconds());

	
        //vglCheckContext(out, VGL_RAM_CONTEXT);
		cv::imwrite("../images/lenaout_rgbtorgba.png", RGBA,saveparams);


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
