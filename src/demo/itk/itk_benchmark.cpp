#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"
#include "../timer.h"


#include <string>

//#define GPU

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
    double t0 = 0.0d;
    double tf = 0.0d;

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
    // Saving Not result
    saveFile((char*) "/tmp/itk_not.dcm", negateFilter->GetOutput());


#ifdef GPU
    // GPU Negative
    typedef itk::GPUUnaryFunctorImageFilter<ImageType,ImageType,
                  Negate<ImageType::PixelType,ImageType::PixelType> > GPUNegateImageFilterType;
 
    GPUNegateImageFilterType::Pointer gpuNegateFilter = GPUNegateImageFilterType::New();
    gpuNegateFilter->SetInput(image);
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


#ifdef GPU
    // GPU Threshold

    typedef itk::GPUBinaryThresholdImageFilter <ImageType, ImageType>
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
//    gpuThresholdFilter->GetOutput();
    saveFile((char*) "/tmp/itk_gpu_thresh.dcm", gpuImage);
#endif

    // Mean
    typedef itk::MeanImageFilter< ImageType, ImageType > MeanFilterType;
    MeanFilterType::Pointer meanFilter = MeanFilterType::New();

    meanFilter = MeanFilterType::New();
    meanFilter->SetInput( image );
    meanFilter->SetRadius( 1 );

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

    // Binomial Blur (aproximation of gaussian blur)
    typedef itk::BinomialBlurImageFilter<ImageType, ImageType> BinomialBlurImageFilterType;
 
    int repetitions = 1;
    BinomialBlurImageFilterType::Pointer blurFilter = BinomialBlurImageFilterType::New();

    blurFilter = BinomialBlurImageFilterType::New();
    blurFilter->SetInput( reader->GetOutput() );
    blurFilter->SetRepetitions( repetitions );

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

#ifdef GPU
    // GPU Blur
    typedef itk::BoxImageFilter< ImageType, ImageType > BoxImageFilterType;
    typedef itk::GPUBoxImageFilter< ImageType, ImageType, BoxImageFilterType > GPUBoxImageFilterType;

    GPUBoxImageFilterType::Pointer GPUBlurFilter = GPUBoxImageFilterType::New();

    GPUBlurFilter->SetInput(gpureader->GetOutput());

    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
      GPUBlurFilter->Modified();
      GPUBlurFilter->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d gpu blur: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
    // Saving GPU Blur result
//    saveFile((char*) "/tmp/itk_gpu_blur.dcm", GPUBlurFilter->GetOutput());

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

    // Erosion 5x5
    StructuringElementType structuringElement5x5;
    radius = 2;
    structuringElement5x5.SetRadius(radius);
    structuringElement5x5.CreateStructuringElement();

    GrayscaleErodeImageFilterType::Pointer erodeFilter5x5;

    erodeFilter5x5 = GrayscaleErodeImageFilterType::New();
    erodeFilter5x5->SetInput(reader->GetOutput());
    erodeFilter5x5->SetKernel(structuringElement5x5);

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

    // Copy
    typedef itk::ImageDuplicator< ImageType > DuplicatorType;
    DuplicatorType::Pointer duplicator;

    duplicator = DuplicatorType::New();
    duplicator->SetInputImage(image);

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

    // Convolution 5x5
    ImageType::Pointer kernel5x5 = ImageType::New();
    convWidth = 5;
    CreateKernel(kernel5x5, convWidth);

    convolutionFilter->SetKernelImage(kernel5x5);

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


#ifdef GPU

    // GPU Mean
  /*typedef itk::GPUMeanImageFilter<ImageType, ImageType> GPUMeanFilterType;
    GPUMeanFilterType::Pointer GPUMean = GPUMeanFilterType::New();

    GPUMean->SetInput(gpureader->GetOutput());
    GPUMean->SetRadius( 1 );

    itkClock.Start();
    TimerStart();
    for(int n = 0; n < operations; n++)
    {
       GPUMean->Modified();
       GPUMean->Update();
    }
    itkClock.Stop();
    printf("Tempo gasto para fazer %d GPU mean blur: %s\n",operations, getTimeElapsedInSeconds());
    tf = itkClock.GetTotal();
    std::cout << "My: "    << (tf - t0) << std::endl;
    t0 = tf;
	*/
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

