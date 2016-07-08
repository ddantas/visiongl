#include "vglImage.h"
#include "vglClImage.h"
#include "vglDcmtkIo.h"
#include "vglClFunctions.h"
#include "demo/timer.h"


int main(int argc, char* argv[])
{

    vglInit(50,50);
    vglClInit();

    if (argc < 2)
    {
        printf("Usage: demo_histogram file.dicom\n");
        printf("bad arguments, read usage again\n");
        exit(1);
    }

    char* image_path = argv[1];

    printf("VisionCL on %s\n\n",image_path);

    //REMEMBER, LOAD A GRAY LEVEL IMAGE
    VglImage* img = vglDcmtkLoadDicom(image_path);
    VglImage* out = vglCreate3dImage(cvSize(img->getWidth(),img->getHeight()),img->depth,img->nChannels,img->getLength());

    int* eq = (int*) malloc(256*sizeof(int));
    for(int i = 0; i < 256; i++)
    {
      if (i < 96)
        eq[i] = 0;
      else if (i < 200)
        eq[i] = i;
      else
        eq[i] = 0;
    }

    //OpenCL histogram
    TimerStart();
    vglClHistogram(img);
    printf("First call to          cl3dHistogram: %s\n", getTimeElapsedInSeconds());
    TimerStart();
    vglCl3dGrayLevelTransform(img,out,eq);
    printf("First call to cl3dGrayLevelTransform: %s\n", getTimeElapsedInSeconds());
    TimerStart();
    vglCl3dGrayLevelTransform(img,out,eq);
    printf("Testing cl3dGrayLevelTransform time: %s\n", getTimeElapsedInSeconds());
    TimerStart();
    int* histogram = vglClHistogram(img);
    printf("Testing cl3dHistogram time: %s\n", getTimeElapsedInSeconds());

    //CPU histogram
    TimerStart();
    int* histogram_cpu_r = (int*) malloc(256*sizeof(int));

    memset(histogram_cpu_r,0x0,256*sizeof(int));

    unsigned char* pixels = (unsigned char*) img->ndarray;

    for(int i = 0; i < img->getWidth()*img->getHeight()*img->getLength();i++)
    {
      histogram_cpu_r[pixels[i]]++;
    }

    printf("Testing Histogram CPU time: %s\n", getTimeElapsedInSeconds());
    //Test if they are equal
    int t = 0;
    for(int i = 0; i < 256; i++)
    {
      t += histogram_cpu_r[i] - histogram[i];
    }
    printf("Total diff %d\n", t);

    vglCl3dHistogramEq(img,out);
    
    vglClDownload(out);
    vglDcmtkSaveDicom("C:/Users/H_DANILO/Dropbox/TCC/TCC_DICOM_HISTEQ.dcm", out, 0);

    

    vglCl3dGrayLevelTransform(img,out,eq);

    vglClDownload(out);
    vglDcmtkSaveDicom("C:/Users/H_DANILO/Dropbox/TCC/TCC_DICOM_GrayLevelTransform.dcm", out, 0);
    
    return 0;
}
