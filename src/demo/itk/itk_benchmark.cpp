#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

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

// GPU Threshold
#include "itkGPUBinaryThresholdImageFilter.h"

// GPU Not
#include "itkGPUUnaryFunctorImageFilter.h"

// GPU Blur
#include "itkGPUBoxImageFilter.h"

#endif




const unsigned int Dimension = 3;
typedef unsigned char PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;
typedef itk::ImageFileWriter< ImageType > WriterType;


void saveFile(char* filename, ImageType* image)
{
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(filename);
    writer->SetInput(image);
    writer->Update();
}


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
    if( argc != 2 )
    {
        std::cerr << "Usage: "<< std::endl;
        std::cerr << argv[0];
        std::cerr << " <InputFileName>";
        std::cerr << std::endl;
        return EXIT_FAILURE;
    }

    // Loading file
    typedef itk::ImageFileReader< ImageType > ReaderType;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[1] );
    reader->Update();
    ImageType::Pointer image = reader->GetOutput();


    // Allocate output image
    ImageType::Pointer output = ImageType::New();
    ImageType::RegionType region = image->GetBufferedRegion();
    output->SetRegions( region );
    output->SetOrigin(  image->GetOrigin()  );
    output->SetSpacing( image->GetSpacing() );
    output->Allocate();


    // Common Threshold
    int lowerThreshold = 100;
    int upperThreshold = 200;

    // Threshold
    typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
       BinaryThresholdImageFilterType;
    
    BinaryThresholdImageFilterType::Pointer thresholdFilter
      = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(reader->GetOutput());
    thresholdFilter->SetLowerThreshold(lowerThreshold);
    thresholdFilter->SetUpperThreshold(upperThreshold);
    thresholdFilter->SetInsideValue(255);
    thresholdFilter->SetOutsideValue(0);
    // Saving Threshold result
    saveFile((char*) "/tmp/itk_thresh.dcm", thresholdFilter->GetOutput());


#ifdef GPU
    // GPU Threshold
    typedef itk::GPUBinaryThresholdImageFilter <ImageType, ImageType>
       GPUBinaryThresholdImageFilterType;
    
    GPUBinaryThresholdImageFilterType::Pointer gpuThresholdFilter
      = GPUBinaryThresholdImageFilterType::New();
    gpuThresholdFilter->SetInput(reader->GetOutput());
    gpuThresholdFilter->SetLowerThreshold(lowerThreshold);
    gpuThresholdFilter->SetUpperThreshold(upperThreshold);
    gpuThresholdFilter->SetInsideValue(255);
    gpuThresholdFilter->SetOutsideValue(0);
    // Saving GPU Threshold result
    saveFile((char*) "/tmp/itk_gpu_thresh.dcm", gpuThresholdFilter->GetOutput());
#endif



    // Negative
    typedef itk::UnaryFunctorImageFilter<ImageType,ImageType,
                  Negate<ImageType::PixelType,ImageType::PixelType> > NegateImageFilterType;
 
    NegateImageFilterType::Pointer negateFilter = NegateImageFilterType::New();
    negateFilter->SetInput(image);
    negateFilter->Update();    
    // Saving Not result
    saveFile((char*) "/tmp/itk_not.dcm", negateFilter->GetOutput());


#ifdef GPU
    // GPU Negative
    typedef itk::GPUUnaryFunctorImageFilter<ImageType,ImageType,
                  Negate<ImageType::PixelType,ImageType::PixelType> > GPUNegateImageFilterType;
 
    //GPUNegateImageFilterType::Pointer gpuNegateFilter = GPUNegateImageFilterType::New();
    //gpuNegateFilter->SetInput(image);
    //gpuNegateFilter->Update();    
    // Saving Not result
    //saveFile("/tmp/itk_gpu_not.dcm", gpuNegateFilter->GetOutput());
#endif


    // Binomial Blur (aproximation of gaussian blur)
    typedef itk::BinomialBlurImageFilter<ImageType, ImageType> BinomialBlurImageFilterType;
 
    int repetitions = 1;
    BinomialBlurImageFilterType::Pointer blurFilter = BinomialBlurImageFilterType::New();
    blurFilter->SetInput( reader->GetOutput() );
    blurFilter->SetRepetitions( repetitions );
    // Saving Blur result
    saveFile((char*) "/tmp/itk_blur.dcm", blurFilter->GetOutput());

#ifdef GPU
    // GPU Blur
    typedef itk::BoxImageFilter< ImageType, ImageType > BoxImageFilterType;
    typedef itk::GPUBoxImageFilter< ImageType, ImageType, BoxImageFilterType > GPUBoxImageFilterType;

    GPUBoxImageFilterType::Pointer GPUBlurFilter = GPUBoxImageFilterType::New();

    GPUBlurFilter->SetInput(reader->GetOutput());
    //GPUBlurFilter->SetRepetitions( repetitions );
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

    GrayscaleErodeImageFilterType::Pointer erodeFilter3x3
      = GrayscaleErodeImageFilterType::New();
    erodeFilter3x3->SetInput(reader->GetOutput());
    erodeFilter3x3->SetKernel(structuringElement3x3);
    // Saving Erosion result
    saveFile((char*) "/tmp/itk_erode3x3.dcm", erodeFilter3x3->GetOutput());

    // Erosion 5x5
    StructuringElementType structuringElement5x5;
    radius = 2;
    structuringElement5x5.SetRadius(radius);
    structuringElement5x5.CreateStructuringElement();

    GrayscaleErodeImageFilterType::Pointer erodeFilter5x5
      = GrayscaleErodeImageFilterType::New();
    erodeFilter5x5->SetInput(reader->GetOutput());
    erodeFilter5x5->SetKernel(structuringElement5x5);
    // Saving Erosion result
    saveFile((char*) "/tmp/itk_erode5x5.dcm", erodeFilter5x5->GetOutput());

    // Copy
    typedef itk::ImageDuplicator< ImageType > DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(image);
    duplicator->Update();
    // Saving Copy result
    saveFile((char*) "/tmp/itk_copy.dcm", duplicator->GetOutput());

    // Convolution
    ImageType::Pointer kernel = ImageType::New();
    CreateKernel(kernel, 3);

    typedef itk::ConvolutionImageFilter<ImageType> ConvolutionImageFilterType;

    ConvolutionImageFilterType::Pointer convolutionFilter = ConvolutionImageFilterType::New();
    convolutionFilter->SetInput(reader->GetOutput());
    convolutionFilter->SetKernelImage(kernel);
    // Saving Convolution result
    saveFile((char*) "/tmp/itk_convolution3x3.dcm", convolutionFilter->GetOutput());


    // Mean
    typedef itk::MeanImageFilter< ImageType, ImageType > MeanFilterType;
    MeanFilterType::Pointer meanFilter = MeanFilterType::New();

    meanFilter->SetInput( image );
    meanFilter->SetRadius( 1 );
    meanFilter->Update();
    // Saving Convolution result
    saveFile((char*) "/tmp/itk_mean3x3.dcm", meanFilter->GetOutput());


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

