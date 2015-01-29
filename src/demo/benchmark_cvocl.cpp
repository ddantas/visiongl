
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ocl/ocl.hpp>

#include "demo/timer.h"

#include <fstream>

//using namespace std;
//using namespace cv;


/*
	argv[1] = input_image_path
	argv[2] = number of operations to execute
*/
int main(int argc, char* argv[])
{

    if (argc != 4)
    {
        printf("\nUsage: demo_benchmark_cvocl lena_1024.tiff 1000 /tmp\n\n");
        printf("In this example, will run the program for lena_1024.tiff in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
        printf("Error: Bad number of arguments = %d. 3 arguments required.\n", argc-1);
        exit(1);
    }

    int nSteps = atoi(argv[2]);
    char* inFilename = argv[1];
    char* outPath = argv[3];
    char* outFilename = (char*) malloc(strlen(outPath) + 200);
    printf("OpenCVCLBenchmark on %s for %d operations\n\n",inFilename,nSteps);
    cv::Mat imgxm = cv::imread(inFilename,1);
    cv::Mat imgRGBA(imgxm.size(), CV_8UC4);
    

    cvtColor(imgxm, imgRGBA, CV_BGR2RGBA);
    cv::ocl::oclMat img(imgRGBA), out(imgRGBA);

    std::vector<int> saveparams;
    //saveparams.push_back(CV_IMWRITE_PNG_COMPRESSION);
    //saveparams.push_back(1);
	
    if (img.data == NULL)
    {
        std::string str("Error: File not found: ");
        str.append(inFilename);
        printf("%s",str.c_str());
    }

    //First call to Blur 3x3
    TimerStart();
    cv::ocl::GaussianBlur(img, out, cv::Size(3,3), 0);
    cv::ocl::finish();
    //ocl::blur(img,out,ksize);
    printf("First call to          Blur 3x3:                %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Blur 3x3
    int p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
	      cv::ocl::GaussianBlur(img, out, cv::Size(3,3), 0);
        //ocl::blur(img,out,ksize);
    }
    cv::ocl::finish();
    printf("Time spent on %8d Blur 3x3:                %s\n", nSteps, getTimeElapsedInSeconds());
    
    cv::Mat saveout(out);
    cvtColor(saveout, saveout, CV_RGBA2BGR);
    sprintf(outFilename, "%s%s", outPath, "/out_cvocl_blur33.tif");
    cv::imwrite(outFilename, saveout, saveparams);

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
    cv::Ptr<cv::ocl::FilterEngine_GPU> filter33 = cv::ocl::createLinearFilter_GPU(CV_8UC4, CV_8UC4, cvkernel33);
    filter33->apply(img, out);
    cv::ocl::finish();
    printf("First call to          Convolution 3x3:         %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Convolution 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        filter33->apply(img, out);
    }
    cv::ocl::finish();
    printf("Time spent on %8d Convolution 3x3:         %s \n", nSteps, getTimeElapsedInSeconds());
    saveout = *new cv::Mat(out);
    cvtColor(saveout, saveout, CV_RGBA2BGR);
    sprintf(outFilename, "%s%s", outPath, "/out_cvocl_conv33.tif");
    cv::imwrite(outFilename, saveout, saveparams);

    //First call to Convolution 5x5
    TimerStart();
    cv::Ptr<cv::ocl::FilterEngine_GPU> filter55 = cv::ocl::createLinearFilter_GPU(CV_8UC4, CV_8UC4, cvkernel55);
    filter55->apply(img, out);
    cv::ocl::finish();
    printf("First call to          Convolution 5x5:         %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Convolution 5x5
    p = 0;
    filter55->apply(img,out);
    TimerStart();
    while (p < nSteps)
    {
        p++;
        filter55->apply(img, out);
    }
    cv::ocl::finish();
    printf("Time spent on %8d Convolution 5x5:         %s\n", nSteps, getTimeElapsedInSeconds());
    saveout = *new cv::Mat(out);
    cvtColor(saveout, saveout, CV_RGBA2BGR);
    sprintf(outFilename, "%s%s", outPath, "/out_cvocl_conv55.tif");
    cv::imwrite(outFilename, saveout,saveparams);

    //First call to Erode 3x3
    TimerStart();
    cv::Mat cverode33 = getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::ocl::erode(img, out, cverode33);
    cv::ocl::finish();
    printf("First call to          Erode 3x3:               %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Erode 3x3
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
	      cv::ocl::erode(img, out, cverode33);
    }
    cv::ocl::finish();
    printf("Time spent on %8d Erode 3x3:               %s\n", nSteps, getTimeElapsedInSeconds());
    saveout = *new cv::Mat(out);
    cvtColor(saveout, saveout, CV_RGBA2BGR);
    sprintf(outFilename, "%s%s", outPath, "/out_cvocl_erode33.tif");
    cv::imwrite(outFilename, saveout,saveparams);
	
    //First call to Invert
    TimerStart();
    cv::ocl::bitwise_not(img,out);
    cv::ocl::finish();
    printf("Fisrt call to          Invert:                  %s\n", getTimeElapsedInSeconds());
    //Total time spent on n operations Invert
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
	      cv::ocl::bitwise_not(img,out);
    }
    cv::ocl::finish();
    printf("Time spent on %8d Invert:                  %s\n", nSteps, getTimeElapsedInSeconds());
    saveout = *new cv::Mat(out);
    cvtColor(saveout, saveout, CV_RGBA2BGR);
    sprintf(outFilename, "%s%s", outPath, "/out_cvocl_invert.tif");
    cv::imwrite(outFilename, saveout,saveparams);

    //First call to Copy GPU->GPU
    TimerStart();
    cv::ocl::oclMat x1(img.size(), CV_8UC4);
    img.copyTo(x1);
    cv::ocl::finish();
    printf("First call to          Copy GPU->GPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->GPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        img.copyTo(x1);
    }
    cv::ocl::finish();
    printf("Time spent on %8d copy GPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());
    saveout = *new cv::Mat(out);
    cvtColor(saveout, saveout, CV_RGBA2BGR);
    sprintf(outFilename, "%s%s", outPath, "/out_cvocl_gpuCopy.tif");
    cv::imwrite(outFilename, saveout, saveparams);

    //First call to Copy CPU->GPU
    cv::ocl::oclMat x;
    p = 0;
    TimerStart();
    img.upload(imgxm);
    cv::ocl::finish();
    printf("First call to          Copy CPU->GPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy CPU->GPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        img.upload(imgxm);
    }
    cv::ocl::finish();
    printf("Time spent on %8d copy CPU->GPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    //First call to Copy GPU->CPU
    cv::Mat x2;
    TimerStart();
    img.download(x2);
    cv::ocl::finish();
    printf("First call to          Copy GPU->CPU:           %s \n", getTimeElapsedInSeconds());
    //Total time spent on n operations Copy GPU->CPU
    p = 0;
    TimerStart();
    while (p < nSteps)
    {
        p++;
        img.download(x2);
    }
    cv::ocl::finish();
    printf("Time spent on %8d copy GPU->CPU:           %s\n", nSteps, getTimeElapsedInSeconds());

    return 0;
}
