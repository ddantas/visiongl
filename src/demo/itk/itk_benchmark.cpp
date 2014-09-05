#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"
#include "../timer.h"


#include <string>

#define GPU
#define GPU_only

// Visualize
//#include "QuickView.h"

// Threshold
#include "itkBinaryThresholdImageFilter.h"

// Not
#include "itkUnaryFunctorImageFilter.h"

// Blur
#include "itkBinomialBlurImageFilter.h"


// Erosion
#include "itkGrayscaleErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

// Copy
#include "itkImageDuplicator.h"

// Convolution
#include "itkConvolutionImageFilter.h"

// Mean
#include "itkMeanImageFilter.h"

#ifdef GPU

// GPU image
#include "itkGPUImage.h"

// GPU Threshold
#include "itkGPUBinaryThresholdImageFilter.h"

// GPU Not
#include "itkGPUUnaryFunctorImageFilter.h"

// GPU Blur
#include "itkGPUBoxImageFilter.h"

// GPU Mean
#include "itkGPUMeanImageFilter.h"

#endif


const unsigned int Dimension = 3;
typedef unsigned char PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;
typedef itk::ImageFileWriter< ImageType > WriterType;
#ifdef GPU
    typedef itk::GPUImage< PixelType, Dimension > GPUImageType;
    typedef itk::ImageFileReader< GPUImageType > GPUReaderType;
    typedef itk::ImageFileWriter< GPUImageType > GPUWriterType;
#endif


void saveFile(char* filename, ImageType* image)
{
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(filename);
    writer->SetInput(image);
    writer->Update();
}

#ifdef GPU
void saveFile(char* filename, GPUImageType* image)
{
    GPUWriterType::Pointer writer = GPUWriterType::New();
    image->UpdateBuffers();
    writer->SetFileName(filename);
    writer->SetInput(image);
    writer->Update();
}
#endif

template< class TInput, class TOutput>
class Negate
{
public:
  Negate() {};
  ~Negate() {};
  bool operator!=( const Negate & ) const
    {
    return false;
    }
  bool operator==( const Negate & other ) const
    {
    return !(*this != other);
    }
  inline TOutput operator()( const TInput & A ) const
    {
      return 255 - (A);
    }
};


void CreateKernel(ImageType::Pointer kernel, unsigned int width)
{
  ImageType::IndexType start;
  start.Fill(0);
 
  ImageType::SizeType size;
  size.Fill(width);
 
  ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
 
  kernel->SetRegions(region);
  kernel->Allocate();
 
  itk::ImageRegionIterator<ImageType> imageIterator(kernel, region);
 
   while(!imageIterator.IsAtEnd())
    {
    //imageIterator.Set(255);
    imageIterator.Set(1);
 
    ++imageIterator;
    }
}



int main( int argc, char* argv[] )
{
    if( argc != 3 )
    {
        std::cerr << "Usage: "<< std::endl;
        std::cerr << argv[0];
        std::cerr << " <InputFileName> n";
        std::cerr << std::endl;
        return EXIT_FAILURE;
    }



    int operations = atoi(argv[2]);
    //sscanf(&operations,"%d",argv[2]);
    //printf("%d\n", operations);

    itk::TimeProbe itkClock;
    double t0 = 0.0;
    double tf = 0.0;

    itk::MultiThreader::SetGlobalDefaultNumberOfThreads(1);

    // Loading file
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[1] );
    reader->Update();
    ImageType::Pointer image = reader->GetOutput();

#ifdef GPU
    GPUReaderType::Pointer gpureader = GPUReaderType::New();
    gpureader->SetFileName( argv[1] );
    gpureader->Update();
    GPUImageType::Pointer gpuImage = gpureader->GetOutput();
#endif


    saveFile((char*) "/tmp/itk_input.dcm", image);

    // Allocate output image
    ImageType::Pointer output = ImageType::New();
    ImageType::RegionType region = image->GetBufferedRegion();
    output->SetRegions( region );
    output->SetOrigin(  image->GetOrigin()  );
    output->SetSpacing( image->GetSpacing() );
    output->Allocate();


    // Negative
    typedef itk::UnaryFunctorImageFilter<ImageType,ImageType,
                  Negate<ImageType::PixelType,ImageType::PixelType> > NegateImageFilterType;
 
    NegateImageFilterType::Pointer negateFilter = NegateImageFilterType::New();

    negateFilter = NegateImageFilterType::New();
    negateFilter->SetInput(image);
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
      negateFilter->Modified();
      negateFilter->Update();    
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d negative: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
#endif
    // Saving Not result
    saveFile((char*) "/tmp/itk_not.dcm", negateFilter->GetOutput());


#ifdef GPU
    // GPU Negative
    typedef itk::GPUUnaryFunctorImageFilter<ImageType,ImageType,
                  Negate<ImageType::PixelType,ImageType::PixelType> > GPUNegateImageFilterType;
 
    GPUNegateImageFilterType::Pointer gpuNegateFilter = GPUNegateImageFilterType::New();
    gpuNegateFilter->SetInput(gpureader->GetOutput());
    gpuNegateFilter->Update();    
    // Saving Not result
    //saveFile("/tmp/itk_gpu_not.dcm", gpuNegateFilter->GetOutput());
#endif

    // Common Threshold
    int lowerThreshold = 100;
    int upperThreshold = 200;


    // Threshold
    typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
       BinaryThresholdImageFilterType;
    
    BinaryThresholdImageFilterType::Pointer thresholdFilter
      = BinaryThresholdImageFilterType::New();

    thresholdFilter = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(reader->GetOutput());
    thresholdFilter->SetLowerThreshold(lowerThreshold);
    thresholdFilter->SetUpperThreshold(upperThreshold);
    thresholdFilter->SetInsideValue(255);
    thresholdFilter->SetOutsideValue(0);
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
	thresholdFilter->Modified();
	thresholdFilter->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d threshold: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    // Saving Threshold result
    saveFile((char*) "/tmp/itk_thresh.dcm", thresholdFilter->GetOutput());
#endif


#ifdef GPU
    // GPU Threshold

    typedef itk::GPUBinaryThresholdImageFilter <GPUImageType, GPUImageType> 
			GPUBinaryThresholdImageFilterType;
    
    GPUBinaryThresholdImageFilterType::Pointer gpuThresholdFilter
      = GPUBinaryThresholdImageFilterType::New();

    gpuThresholdFilter->SetInput(gpureader->GetOutput());
    gpuThresholdFilter->SetLowerThreshold(lowerThreshold);
    gpuThresholdFilter->SetUpperThreshold(upperThreshold);
    gpuThresholdFilter->SetInsideValue(255);
    gpuThresholdFilter->SetOutsideValue(0);

    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
      gpuThresholdFilter->Modified();
      gpuThresholdFilter->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d GPU threshold: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    // Saving GPU Threshold result
    gpuThresholdFilter->GetOutput()->UpdateBuffers();
    saveFile((char*) "/tmp/itk_gpu_thresh.dcm", gpuThresholdFilter->GetOutput());
#endif

    // Mean
    typedef itk::MeanImageFilter< ImageType, ImageType > MeanFilterType;
    MeanFilterType::Pointer meanFilter = MeanFilterType::New();

    meanFilter = MeanFilterType::New();
    meanFilter->SetInput( image );
    meanFilter->SetRadius( 1 );
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
       meanFilter->Modified();
       meanFilter->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d mean blur: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    // Saving Convolution result
    saveFile((char*) "/tmp/itk_mean3x3.dcm", meanFilter->GetOutput());
#endif

    // Binomial Blur (aproximation of gaussian blur)
    typedef itk::BinomialBlurImageFilter<ImageType, ImageType> BinomialBlurImageFilterType;
 
    int repetitions = 1;
    BinomialBlurImageFilterType::Pointer blurFilter = BinomialBlurImageFilterType::New();

    blurFilter = BinomialBlurImageFilterType::New();
    blurFilter->SetInput( reader->GetOutput() );
    blurFilter->SetRepetitions( repetitions );
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
      blurFilter->Modified();
      blurFilter->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d blur: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    // Saving Blur result
    saveFile((char*) "/tmp/itk_blur.dcm", blurFilter->GetOutput());
#endif


#ifdef GPU
    // GPU Blur
    typedef itk::BoxImageFilter< GPUImageType, GPUImageType > BoxImageFilterType;
    typedef itk::GPUBoxImageFilter< GPUImageType, GPUImageType, BoxImageFilterType > GPUBoxImageFilterType;

    GPUBoxImageFilterType::Pointer GPUBlurFilter = GPUBoxImageFilterType::New();

	//ImageType::SizeType indexRadius;
	//indexRadius[0] = 2;
	//indexRadius[1] = 2;
	//indexRadius[2] = 2;

    GPUBlurFilter->SetInput(gpureader->GetOutput());
	//GPUBlurFilter->SetRadius(indexRadius);
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
        GPUBlurFilter->Update();
        GPUBlurFilter->Modified();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d gpu blur: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    GPUBlurFilter->GetOutput()->UpdateBuffers();
    // Saving GPU Blur result
    saveFile((char*) "/tmp/itk_gpu_blur.dcm", GPUBlurFilter->GetOutput());

#endif


    //Erosion Common
    typedef itk::BinaryBallStructuringElement<
      ImageType::PixelType, 3>                  StructuringElementType;
    typedef itk::GrayscaleErodeImageFilter <ImageType, ImageType, StructuringElementType>
      GrayscaleErodeImageFilterType;
    unsigned int radius;

    // Erosion 3x3
    StructuringElementType structuringElement3x3;
    radius = 1;
    structuringElement3x3.SetRadius(radius);
    structuringElement3x3.CreateStructuringElement();

    GrayscaleErodeImageFilterType::Pointer erodeFilter3x3;

    erodeFilter3x3= GrayscaleErodeImageFilterType::New();
    erodeFilter3x3->SetInput(reader->GetOutput());
    erodeFilter3x3->SetKernel(structuringElement3x3);
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
        erodeFilter3x3->Modified();
        erodeFilter3x3->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d erosion 3x3: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;

    // Saving Erosion result
    saveFile((char*) "/tmp/itk_erode3x3.dcm", erodeFilter3x3->GetOutput());
#endif

    // Erosion 5x5
    StructuringElementType structuringElement5x5;
    radius = 2;
    structuringElement5x5.SetRadius(radius);
    structuringElement5x5.CreateStructuringElement();

    GrayscaleErodeImageFilterType::Pointer erodeFilter5x5;

    erodeFilter5x5 = GrayscaleErodeImageFilterType::New();
    erodeFilter5x5->SetInput(reader->GetOutput());
    erodeFilter5x5->SetKernel(structuringElement5x5);
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
      erodeFilter5x5->Modified();
      erodeFilter5x5->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d erosion 5x5: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;

    // Saving Erosion result
    saveFile((char*) "/tmp/itk_erode5x5.dcm", erodeFilter5x5->GetOutput());
#endif

    // Copy
    typedef itk::ImageDuplicator< ImageType > DuplicatorType;
    DuplicatorType::Pointer duplicator;

    duplicator = DuplicatorType::New();
    duplicator->SetInputImage(image);
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    { 
       duplicator->Modified();
       duplicator->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d copias cpu: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    // Saving Copy result
    saveFile((char*) "/tmp/itk_copy.dcm", duplicator->GetOutput());
#endif

    // Convolution common
    typedef itk::ConvolutionImageFilter<ImageType> ConvolutionImageFilterType;

    ConvolutionImageFilterType::Pointer convolutionFilter;

    convolutionFilter = ConvolutionImageFilterType::New();
    convolutionFilter->SetInput(reader->GetOutput());
    int convWidth;

    // Convolution 3x3
    ImageType::Pointer kernel3x3 = ImageType::New();
    convWidth = 3;
    CreateKernel(kernel3x3, convWidth);

    convolutionFilter->SetKernelImage(kernel3x3);
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {  
       convolutionFilter->Modified();
       convolutionFilter->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d convolucoes 3x3 cpu: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    // Saving Convolution result
    saveFile((char*) "/tmp/itk_convolution3x3.dcm", convolutionFilter->GetOutput());
#endif

    // Convolution 5x5
    ImageType::Pointer kernel5x5 = ImageType::New();
    convWidth = 5;
    CreateKernel(kernel5x5, convWidth);

    convolutionFilter->SetKernelImage(kernel5x5);
#ifndef GPU_only
    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {  
       convolutionFilter->Modified();
       convolutionFilter->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d convolucoes 5x5 cpu: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    // Saving Convolution result
    saveFile((char*) "/tmp/itk_convolution5x5.dcm", convolutionFilter->GetOutput());
#endif

#ifdef GPU

    // GPU Mean
    typedef itk::GPUMeanImageFilter<GPUImageType, GPUImageType> GPUMeanFilterType;
	GPUMeanFilterType::Pointer GPUMean = GPUMeanFilterType::New();
	GPUMean->SetInput(gpureader->GetOutput());
	GPUMean->SetRadius( 1 );
	TimerStart();
	for(int n = 0; n < operations; n++)
	{
	   GPUMean->Update();
	   GPUMean->Modified();
	}
    itkClock.Stop();
    printf("Tempo gasto para fazer %d GPU mean blur: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
	GPUMean->GetOutput()->UpdateBuffers();
    saveFile((char*) "/tmp/itk_gpu_blurmean.dcm", GPUMean->GetOutput());
#endif

    // Visualize
    /*
    QuickView viewer;
    viewer.AddImage<ImageType>(
      image,true,
      itksys::SystemTools::GetFilenameName(argv[1]));  
    std::stringstream desc;
    desc << "ITK QuickView: " 
         << argv[1];
    viewer.Visualize();
    */

    // Saving input image as is
    saveFile((char*) "/tmp/itk_input.dcm", image);


    return EXIT_SUCCESS;
}

