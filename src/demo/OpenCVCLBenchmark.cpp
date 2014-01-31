#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ocl/ocl.hpp>

#include "demo/timer.h"
#include <fstream>

using namespace std;
using namespace cv;

int main()
{
	char* image_path = (char*) "../images/lena_1024.tif";
	cv::Mat imgxm = imread(image_path,1);

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
	printf("Primeira chamada de Blur: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 blur 3x3 sem a cria��o da opera��o
	int p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		ocl::blur(img,out,ksize);
	}
	printf("Tempo gasto para fazer 1000 Blur 3x3: %s\n", getTimeElapsedInSeconds());

		cv::Mat saveout(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
        cv::imwrite("../images/lenaout_blur33.png", saveout,saveparams);
		
	//Primeira chamada a Convolution
	
    Mat cvkernel33 = (Mat_<float>(3,3) << 1/9.0, 1/9.0, 1/9.0, 
                                          1/9.0, 1/9.0, 1/9.0, 
	                                      1/9.0, 1/9.0, 1/9.0  );

   Mat cvkernel55 = (Mat_<float>(5,5) << 1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                         1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                         1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                         1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                         1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0  );

	
	TimerStart();
	cv::Ptr<ocl::FilterEngine_GPU> filter33 = ocl::createLinearFilter_GPU(CV_8UC4,CV_8UC4, cvkernel33);
	cv::Ptr<ocl::FilterEngine_GPU> filter55 = ocl::createLinearFilter_GPU(CV_8UC4,CV_8UC4, cvkernel55);
	filter33->apply(img,out);
	printf("Primeira chamada da Convolucao com kernel 3x3: %s\n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 convolu��es 3x3 sem a cria��o da opera��o
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

	//Mede o tempo para 1000 convolu��es 5x5 sem a cria��o da opera��o
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
	Mat cverode33 = getStructuringElement(MORPH_CROSS, Size( 3, 3 ));
	ocl::erode(img,out,cverode33);
	
	printf("Primeira chamada da Erode: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 thresholds sem a cria��o da opera��o
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		ocl::erode(img,out,cverode33);
	}
	printf("Tempo gasto para fazer 1000 Erode: %s\n", getTimeElapsedInSeconds());

		saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("../images/lenaout_thresh.png", saveout,saveparams);
	
	//Primeira chamada a vglClInvert
	TimerStart();
	ocl::bitwise_not(img,out);
	printf("Primeira chamada da Invert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 invert sem a cria��o da opera��o
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
	
	img.copyTo(out);
	printf("Primeira chamada da Copy: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para 1000 copia GPU->GPU
	p = 0;
	TimerStart();
	ocl::oclMat x1(img.size(), CV_8UC4);
	while (p < 1000)
	{
		p++;
		img.copyTo(x1);
	}
	printf("Tempo gasto para fazer 1000 copia GPU->GPU: %s\n", getTimeElapsedInSeconds());

        saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("../images/lenaout_copy.png", saveout,saveparams);

	//Mede o tempo para 1000 copia CPU->GPU
	
	ocl::oclMat x;
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		img.upload(imgxm);
	}
	printf("Tempo gasto para fazer 1000 copia CPU->GPU(Upload) %s\n", getTimeElapsedInSeconds());

	//Mede o tempo para 1000 copia GPU->CPU
	cv::Mat x2;
	p = 0;
	TimerStart();
	while (p < 1000)
	{
		p++;
		img.download(x2);
	}
	printf("Tempo gasto para fazer 1000 copia GPU->CPU(Download) %s\n", getTimeElapsedInSeconds());

	system("pause");
	return 0;
}