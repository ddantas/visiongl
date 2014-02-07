#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ocl/ocl.hpp>

#include "demo/timer.h"
#include <fstream>

using namespace std;
using namespace cv;


/*
	argv[1] = input_image_path
	argv[2] = number of operations to execute
*/
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: OpenCVCLBenchmark lena_1024.tif 1000\n");
		printf("for this example, will run the program for lena_1024.tif using 1000 operations each\n");
		printf("bad arguments, read usage again\n");
		exit(1);
	}
	int limite = atoi(argv[2]);
	char* image_path = argv[1];
	cv::Mat imgxm = imread(image_path,1);

	cvtColor(imgxm,imgxm,CV_BGR2RGBA);
	ocl::oclMat img(imgxm), out(imgxm);

	vector<int> saveparams;
	saveparams.push_back(CV_IMWRITE_PNG_COMPRESSION);
	saveparams.push_back(1);
	
	if (img.data == NULL)
	{
		std::string str("cvLoadImage/File not found: ");
		str.append(image_path);
		printf("%s",str.c_str());
	}
	
	//Primeira chamada a blurSq3
	Size ksize(3,3);
	TimerStart();
	ocl::GaussianBlur(img,out,Size(3,3),0);
	//ocl::blur(img,out,ksize);
	printf("Primeira chamada de Blur: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para limite blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		//ocl::blur(img,out,ksize);
		ocl::GaussianBlur(img,out,Size(3,3),0);
	}
	printf("Tempo gasto para fazer %d Blur 3x3: %s\n", limite, getTimeElapsedInSeconds());

		cv::Mat saveout(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
        cv::imwrite("images/lenaout_blur33.png", saveout,saveparams);
		
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
	//Mede o tempo para limite convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		filter33->apply(img,out);
	}
	printf("Tempo gasto para fazer %d convolucoes 3x3: %s \n", limite, getTimeElapsedInSeconds());
		
		saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("images/lenaout_conv33.png", saveout,saveparams);

	//Mede o tempo para limite convoluções 5x5 sem a criação da operação
	p = 0;
	filter55->apply(img,out);
	TimerStart();
	while (p < limite)
	{
		p++;
		filter55->apply(out,out);
	}
	printf("Tempo gasto para fazer %d convolucoes 5x5: %s\n", limite, getTimeElapsedInSeconds());
		
		saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("images/lenaout_conv55.png", saveout,saveparams);

	//Primeira chamada a threshold
	TimerStart();
	Mat cverode33 = getStructuringElement(MORPH_CROSS, Size( 3, 3 ));
	ocl::erode(img,out,cverode33);
	
	printf("Primeira chamada da Erode: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para limite thresholds sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		ocl::erode(img,out,cverode33);
	}
	printf("Tempo gasto para fazer %d Erosions: %s\n", limite, getTimeElapsedInSeconds());

		saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("images/lenaout_erosion.png", saveout,saveparams);
	
	//Primeira chamada a vglClInvert
	TimerStart();
	ocl::bitwise_not(img,out);
	printf("Primeira chamada da Invert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para limite invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		ocl::bitwise_not(img,out);
	}
	printf("Tempo gasto para fazer %d Invert: %s\n",limite, getTimeElapsedInSeconds());

		saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("images/lenaout_invert.png", saveout,saveparams);

	TimerStart();
	
	img.copyTo(out);
	printf("Primeira chamada da Copy: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para limite copia GPU->GPU
	p = 0;
	TimerStart();
	ocl::oclMat x1(img.size(), CV_8UC4);
	while (p < limite)
	{
		p++;
		img.copyTo(x1);
	}
	printf("Tempo gasto para fazer %d copia GPU->GPU: %s\n", limite, getTimeElapsedInSeconds());

        saveout = *new cv::Mat(out);
		cvtColor(saveout,saveout,CV_RGBA2BGR);
		cv::imwrite("images/lenaout_copy.png", saveout,saveparams);

	//Mede o tempo para limite copia CPU->GPU
	
	ocl::oclMat x;
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.upload(imgxm);
	}
	printf("Tempo gasto para fazer %d copia CPU->GPU(Upload) %s\n", limite, getTimeElapsedInSeconds());

	//Mede o tempo para limite copia GPU->CPU
	cv::Mat x2;
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.download(x2);
	}
	printf("Tempo gasto para fazer %d copia GPU->CPU(Download) %s\n", limite, getTimeElapsedInSeconds());
	return 0;
}
