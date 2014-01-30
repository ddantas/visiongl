#define __OPENCL__
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/ocl.hpp>
#include <opencv2/ocl/ocl.hpp>

#include "demo/timer.h"
#include <fstream>

using namespace std;
using namespace cv;

int main()
{
	char* image_path = (char*) "../images/lena_std.tif";
	IplImage* imgx = cvLoadImage(image_path,1);
	cv::Mat imgxm = cv::Mat(imgx);
	cvtColor(imgxm,imgxm,CV_BGR2RGBA);
	ocl::oclMat img(imgxm), out(imgxm);

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
	
	//Primeira chamada a blurSq3
	Size ksize(3,3);
	TimerStart();
	ocl::blur(img,out,ksize);
	printf("Primeira chamada da cvSmooth: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		ocl::blur(img,out,ksize);
	}
	printf("Tempo gasto para fazer 1000 blur 3x3: %s\n", getTimeElapsedInSeconds());

		cv::Mat saveout(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
        cv::imwrite("../images/lenaout_blur33.png", saveout,saveparams);
		

        // Kernels para convolucao
	float kernel33[3][3]    = { {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f},
                                    {2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f},
                                    {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f}, }; //blur 3x3
	float kernel55[5][5]    = { {1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f},
                                    {4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f},
                                    {6.0f/256.0f, 24.0f/256.0f, 36.0f/256.0f, 24.0f/256.0f, 6.0f/256.0f},
                                    {4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f},
                                    {1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f}, }; //blur 5x5


	//Primeira chamada a Convolution
	
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
	cv::Ptr<ocl::FilterEngine_GPU> filter33 = ocl::createLinearFilter_GPU(CV_8UC4,CV_8UC4,cvkernel33);
	cv::Ptr<ocl::FilterEngine_GPU> filter55 = ocl::createLinearFilter_GPU(CV_8UC4,CV_8UC4,cvkernel55);
	filter33->apply(img,out);
	printf("Primeira chamada da Filter2D com kernel 3x3: %s\n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		filter33->apply(img,out);
	}
	printf("Tempo gasto para fazer 1000 convolucoes 3x3: %s \n", getTimeElapsedInSeconds());
		
		saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("../images/lenaout_conv33.png", saveout,saveparams);

	//Mede o tempo para 1000 convoluções 5x5 sem a criação da operação
	p = 0;
	filter55->apply(img,out);
	TimerStart();
	while (p < 1000)
	{
		p++;
		filter55->apply(out,out);
	}
	printf("Tempo gasto para fazer 1000 convolucoes 5x5: %s\n", getTimeElapsedInSeconds());
		
		saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("../images/lenaout_conv55.png", saveout,saveparams);

	//Primeira chamada a threshold
	TimerStart();
	ocl::oclMat aux(img);
	ocl::cvtColor(img,img,CV_RGBA2GRAY);
	ocl::threshold(img,out,128,255);
	
	printf("Primeira chamada da cvThreshold: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 thresholds sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		ocl::threshold(img,out,128,255);
	}
	printf("Tempo gasto para fazer 1000 threshold: %s\n", getTimeElapsedInSeconds());

		saveout = *new cv::Mat(out);
		//cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("../images/lenaout_thresh.png", saveout,saveparams);
	img = aux;
	//Primeira chamada a vglClInvert
	TimerStart();
	ocl::bitwise_not(img,out);
	printf("Primeira chamada da Invert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		ocl::bitwise_not(img,out);
	}
	printf("Tempo gasto para fazer 1000 Invert: %s\n", getTimeElapsedInSeconds());

		saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("../images/lenaout_invert.png", saveout,saveparams);

	TimerStart();
	/*cv::Mat cpmat(1,1,CV_8UC1,Scalar(1));
	cv::Ptr<ocl::FilterEngine_GPU> copy = ocl::createLinearFilter_GPU(CV_8UC4,CV_8UC4,cpmat);
	copy->apply(img,out);*/
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
	printf("Tempo gasto para fazer 1000 copia GPU->GPU: %s\n", getTimeElapsedInSeconds());

        saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("../images/lenaout_copy.png", saveout,saveparams);

	//Mede o tempo para 1000 copia CPU->GPU
	img.create(Size(10,10),CV_8UC4);
	ocl::oclMat x;
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		x = *new ocl::oclMat(imgxm);
	}
	printf("Tempo gasto para fazer 1000 copia CPU->GPU %s\n", getTimeElapsedInSeconds());

	//Mede o tempo para 1000 copia GPU->CPU
	cv::Mat x2;
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		x2 = *new cv::Mat(img);
	}
	printf("Tempo gasto para fazer 1000 copia GPU->CPU %s\n", getTimeElapsedInSeconds());

	system("pause");
	return 0;
}
