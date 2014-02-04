#define __OPENCL__
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "demo/timer.h"
#include <ctime>

#include <fstream>

using namespace cv;


int main(int argc, char* argv[])
{
	printf("Usage: OpenCVCLBenchmark lena_1024.tiff 1000 output.out\n");
	printf("for this example, will run the program for lena_1024.tiff using 1000 operations each and output benchmark at output.out\n");
	if (argc != 4)
	{
		printf("bad arguments, read usage again %d \n", argc);
		exit(1);
	}
	int limite = atoi(argv[2]);
	char* image_path = argv[1];
	cv::Mat img = imread(image_path,1);
	FILE* f = fopen(argv[3],"w");
	
	vector<int> saveparams;
	saveparams.push_back(CV_IMWRITE_PNG_COMPRESSION);
	saveparams.push_back(1);
	
	if (img.data == NULL)
	{
        std::string str("cvLoadImage/File not found: ");
		str.append(image_path);
		printf("%s",str.c_str());
	}
	cv::Mat out(img.cols,img.rows,CV_8UC3);

	//Primeira chamada a blurSq3
	TimerStart();
	cv::GaussianBlur(img,out,Size(3,3),0);
	fprintf(f,"Primeira chamada da Blur: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" blur 3x3 sem a criação da operação
	int p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cv::GaussianBlur(img,out,Size(3,3),0);
	}
	fprintf(f,"Tempo gasto para fazer %d Blur 3x3: %s\n",limite, getTimeElapsedInSeconds());

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
	fprintf(f,"Primeira chamada da Filter2D com kernel 3x3: %s\n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" convoluções 3x3 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cv::filter2D(img,out,-1, cvkernel33);
	}
	fprintf(f,"Tempo gasto para fazer %d convolucoes 3x3: %s \n",limite, getTimeElapsedInSeconds());


        cv::imwrite("../images/lenaout_conv33.png", out,saveparams);

	//Mede o tempo para "limite" convoluções 5x5 sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cv::filter2D(img,out,-1, cvkernel55);
	}
	fprintf(f,"Tempo gasto para fazer %d convolucoes 5x5: %s\n",limite, getTimeElapsedInSeconds());

    cv::imwrite("../images/lenaout_conv55.png", out,saveparams);
	
	//Primeira chamada a threshold
	TimerStart();
	Mat cverode33 = getStructuringElement(MORPH_CROSS, Size( 3, 3 ));
	cv::erode(img,out,cverode33);
	fprintf(f,"Primeira chamada da cv::Erode : %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" thresholds sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cv::erode(img,out,cverode33);
	}
	fprintf(f,"Tempo gasto para fazer %d Erosions: %s\n",limite, getTimeElapsedInSeconds());


    cv::imwrite("../images/lenaout_erode33.png", out,saveparams);
	

	//invert
	TimerStart();
	
	cv::bitwise_not(img,out);
	fprintf(f,"Primeira chamada da Invert: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" invert sem a criação da operação
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cv::bitwise_not(img,out);
	}
	fprintf(f,"Tempo gasto para fazer %d Invert: %s\n",limite, getTimeElapsedInSeconds());

	    //vglCheckContext(out, VGL_RAM_CONTEXT);
		cv::imwrite("../images/lenaout_invert33.png", out,saveparams);

	//Primeira chamada a vglClCopy
	TimerStart();
	img.copyTo(out);
	fprintf(f,"Primeira chamada da cvCopy: %s \n", getTimeElapsedInSeconds());
	//Mede o tempo para "limite" copia GPU->GPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		img.copyTo(out);
	}
	fprintf(f,"Tempo gasto para fazer %d copia CPU->CPU: %s\n",limite, getTimeElapsedInSeconds());

	cv::imwrite("../images/lenaout_copy.png", out,saveparams);

	cv::Mat gray(img.cols,img.rows,CV_8UC1);	
	//Mede o tempo para "limite" conversão RGB->Grayscale na CPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cvtColor(img,gray, CV_BGR2GRAY);
	}
	fprintf(f,"Tempo gasto para fazer %d conversões BGR->Gray: %s\n",limite, getTimeElapsedInSeconds());

	cv::imwrite("../images/lenaout_rgbtogray.png", gray,saveparams);

	cv::Mat RGBA(img.cols,img.rows,CV_8UC4);	
	//Mede o tempo para "limite" conversão RGB->RGBA na CPU
	p = 0;
	TimerStart();
	while (p < limite)
	{
		p++;
		cvtColor(img,RGBA, CV_BGR2RGBA);
	}
	fprintf(f,"Tempo gasto para fazer %d conversões BGR->RGBA: %s\n",limite, getTimeElapsedInSeconds());

	cv::imwrite("../images/lenaout_rgbtorgba.png", RGBA,saveparams);

	fclose(f);
	return 0;
}
