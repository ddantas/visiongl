
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "demo/timer.h"

#include <fstream>
#include <vector>

//using namespace cv;

int main(int argc, char* argv[])
{

    if (argc != 4)
    {
        printf("\nUsage: demo_benchmark_cv lena_1024.tiff 1000 /tmp\n\n");
        printf("In this example, will run the program for lena_1024.tiff in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
        printf("Error: Bad number of arguments = %d. 3 arguments required.\n", argc-1);
        exit(1);
    }

    int nSteps = atoi(argv[2]);
    char* inFilename = argv[1];
    char* outPath = argv[3];
    char* outFilename = (char*) malloc(strlen(outPath) + 200);
    printf("OpenCV on %s, %d operations\n\n", inFilename, nSteps);
    cv::Mat img = cv::imread(inFilename, 1);
	
    std::vector<int> saveparams;
    //saveparams.push_back(CV_IMWRITE_PNG_COMPRESSION);
    //saveparams.push_back(1);
	
    if (img.data == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s",str.c_str());
    }
    cv::Mat out(img.cols, img.rows, CV_8UC3);

    //First call to Blur 3x3
    TimerStart();
    cv::GaussianBlur(img, out, cv::Size(3,3), 0);
    printf("First call to          Blur 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Blur 3x3
    int p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cv::GaussianBlur(img, out, cv::Size(3,3), 0);
    }
    printf("Time spent on %8d Blur 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());
    sprintf(outFilename, "%s%s", outPath, "/out_cv_blur33.tif");
    cv::imwrite(outFilename, out, saveparams);

    // Convolution kernels
    cv::Mat cvkernel33 = (cv::Mat_<float>(3,3) << 1/9.0, 1/9.0, 1/9.0, 
                                                  1/9.0, 1/9.0, 1/9.0, 
	                                          1/9.0, 1/9.0, 1/9.0  );

    cv::Mat cvkernel55 = (cv::Mat_<float>(5,5) << 1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                          1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                          1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                          1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0,
                                          1/25.0, 1/25.0, 1/25.0, 1/25.0, 1/25.0  );

    //First call to Convolution 3x3
    TimerStart();
    cv::filter2D(img, out, -1, cvkernel33);
    printf("First call to          Convolution 3x3:         %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Convolution 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cv::filter2D(img, out, -1, cvkernel33);
    }
    printf("Time spent on %8d Convolution 3x3:         %s \n", nSteps, getTimeElapsedInSeconds());
    sprintf(outFilename, "%s%s", outPath, "/out_cv_conv33.tif");
    cv::imwrite(outFilename, out, saveparams);

    //First call to Convolution 5x5
    TimerStart();
    cv::filter2D(img, out, -1, cvkernel55);
    printf("First call to          Convolution 5x5:         %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Convolution 5x5
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cv::filter2D(img,out,-1, cvkernel55);
    }
    printf("Time spent on %8d Convolution 5x5:         %s\n", nSteps, getTimeElapsedInSeconds());
    sprintf(outFilename, "%s%s", outPath, "/out_cv_conv55.tif");
    cv::imwrite(outFilename, out, saveparams);
	
    //First call to Erode 3x3
    TimerStart();
    cv::Mat cverode33 = getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::erode(img,out,cverode33);
    printf("First call to          Erode 3x3:               %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Erode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cv::erode(img,out,cverode33);
    }
    printf("Time spent on %8d Erode 3x3:               %s\n", nSteps, getTimeElapsedInSeconds());
    sprintf(outFilename, "%s%s", outPath, "/out_cv_erode33.tif");
    cv::imwrite(outFilename, out,saveparams);
	
    //First call to Invert
    TimerStart();
    cv::bitwise_not(img,out);
    printf("Fisrt call to          Invert:                  %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Invert
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cv::bitwise_not(img,out);
    }
    printf("Time spent on %8d Invert:                  %s\n", nSteps, getTimeElapsedInSeconds());
    sprintf(outFilename, "%s%s", outPath, "/out_cv_invert.tif");
    cv::imwrite(outFilename, out, saveparams);

    //First call to Copy CPU->CPU
    TimerStart();
    img.copyTo(out);
    printf("First call to          Copy CPU->CPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy CPU->CPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        img.copyTo(out);
    }
    printf("Time spent on %8d copy CPU->CPU:           %s\n", nSteps, getTimeElapsedInSeconds());
    sprintf(outFilename, "%s%s", outPath, "/out_cv_cpuCopy.tif");
    cv::imwrite(outFilename, out,saveparams);

    //First call to Convert BGR->Gray
    TimerStart();
    img.copyTo(out);
    printf("First call to          Convert BGR->Gray:       %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Convert BGR->Gray
    cv::Mat gray(img.cols, img.rows, CV_8UC1);
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cvtColor(img,gray, CV_BGR2GRAY);
    }
    printf("Time spent on %8d Convert BGR->Gray:       %s\n", nSteps, getTimeElapsedInSeconds());
    sprintf(outFilename, "%s%s", outPath, "/out_cv_bgrToGray.tif");
    cv::imwrite(outFilename, gray, saveparams);


    //First call to Convert BGR->RGBA
    TimerStart();
    cv::Mat RGBA(img.cols, img.rows, CV_8UC4);	
    printf("First call to          Convert BGR->RGBA:       %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Convert BGR->RGBA
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        cvtColor(img,RGBA, CV_BGR2RGBA);
    }
    printf("Time spent on %8d Convert BGR->RGBA:       %s\n", nSteps, getTimeElapsedInSeconds());
    sprintf(outFilename, "%s%s", outPath, "/out_cv_bgrToRgba.tif");
    cv::imwrite(outFilename, RGBA,saveparams);


    return 0;
}
