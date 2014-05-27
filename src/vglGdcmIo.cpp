#ifdef __GDCM__

#include <vglGdcmIo.h>
#include <gdcmImageReader.h>
#include <gdcmImage.h>
#include <gdcmWriter.h>
#include <gdcmAttribute.h>
#include <gdcmImageWriter.h>
#include <gdcmImageChangeTransferSyntax.h>
#include "gdcmImageChangeTransferSyntax.h"
#include "gdcmImageChangePhotometricInterpretation.h"
#include "gdcmPhotometricInterpretation.h"

#include <iostream>
#include <fstream>

/** \brief Convert ybr array with 3 bytes to rgb array with 3 bytes.
  */

inline void YBR2RGB(unsigned char rgb[3], unsigned char ybr[3])
{
   const double Y  = ybr[0];
   const double Cb = ybr[1] - 128.0;
   const double Cr = ybr[2] - 128.0;
   const double r = Y                 + 1.402    * Cr;
   const double g = Y - 0.344136 * Cb - 0.714136 * Cr;
   const double b = Y + 1.772    * Cb;
   double R = r < 0 ? 0 : r;
   R = R > 255 ? 255 : R;
   double G = g < 0 ? 0 : g;
   G = G > 255 ? 255 : G;
   double B = b < 0 ? 0 : b;
   B = B > 255 ? 255 : B;
   rgb[0] = R;
   rgb[1] = G;
   rgb[2] = B;
}

/** Function for loading DICOM images with GDCM library
  */

VglImage vglGdcmLoadDicom(char* filename, char* outfilename)
{
  gdcm::ImageReader reader;
  reader.SetFileName(filename);
  if(!reader.Read())
      std::cerr << "Could not read: " << filename << std::endl;
  
  // The output of gdcm::Reader is a gdcm::File
  //gdcm::File &file = reader.GetFile();

  // the dataset is the the set of element we are interested in:
  //gdcm::DataSet &ds = file.GetDataSet();

  gdcm::Image &image = reader.GetImage();
  //image.Print( std::cout );
  
  gdcm::PixelFormat pixelformat = image.GetPixelFormat();
  VglImage imagevgl;

  imagevgl.filename = (char *) malloc(strlen(filename));
  imagevgl.filename = filename;
  imagevgl.shape[0] = image.GetColumns(); // width/columns
  imagevgl.shape[1] = image.GetRows(); // height/rows
  imagevgl.shape[2] = (image.GetDimensions())[2]; // number of frames
  imagevgl.nChannels = pixelformat.GetSamplesPerPixel(); // number of channels of image 
  imagevgl.ndim = image.GetNumberOfDimensions(); // number of dimensions

  int ndarraySize = imagevgl.shape[0]*imagevgl.shape[1]*imagevgl.shape[2]*imagevgl.nChannels;

  if(pixelformat.GetBitsAllocated() == 16)
     ndarraySize = ndarraySize*2;
  
  char* buffer = (char*) malloc(ndarraySize);
  image.GetBuffer(buffer);
  imagevgl.ndarray = buffer; // pixels of image

  if(pixelformat.GetBitsAllocated() == 8) //depth of image
     imagevgl.depth = CV_8U; // 8 bits
     else
       if(pixelformat.GetBitsAllocated() == 16) 
	  imagevgl.depth = CV_16U; // 16 bits
       else
	 if(pixelformat.GetBitsAllocated() == 32) 
	    imagevgl.depth = CV_32S; // 32 bits       
       
  

  /*printf("\n\nColumns: %d\nRows: %d\nFrames: %d\nDepth: %d\nChannels: %d\nndim: %d\n\n", imagevgl.shape[0], imagevgl.shape[1], imagevgl.shape[2], imagevgl.depth, imagevgl.nChannels, imagevgl.ndim);*/

  gdcm::PhotometricInterpretation PI;
  PI = image.GetPhotometricInterpretation();

  if(imagevgl.nChannels == 3)
     if(PI == gdcm::PhotometricInterpretation::YBR_FULL_422) 
        for(int i = 0; i < ndarraySize/3; i++)
        {
	   unsigned char* rgb = (unsigned char*) malloc(3);
	   unsigned char* ybr = &((unsigned char*) (imagevgl.ndarray))[3*i];
	   YBR2RGB(rgb, ybr);
	   //printf("i = %d, imag.ndarray = %d , %d, %d ; rgb = %d , %d, %d\n\n", i, ybr[0], ybr[1], ybr[2], rgb[0], rgb[1], rgb[2]);
	   memcpy(imagevgl.ndarray+3*i, rgb, 3);
	}
     else
       if(!(PI == gdcm::PhotometricInterpretation::RGB))
	  printf("This format is not supported"); 
  
  return imagevgl;
}
  

/** Function for saving uncompressed DICOM images with GDCM library
  */

int vglGdcmSaveDicom(VglImage imagevgl, char* outfilename)
{
  gdcm::ImageReader reader;
  reader.SetFileName(imagevgl.filename);
  if(!reader.Read())
      std::cerr << "Could not read: " << imagevgl.filename << std::endl;
  
  // The output of gdcm::Reader is a gdcm::File
  //gdcm::File &file = reader.GetFile();

  // the dataset is the the set of element we are interested in:
  //gdcm::DataSet &ds = file.GetDataSet();

  gdcm::Image &image = reader.GetImage();
  //image.Print( std::cout );

  int ndarraySize = imagevgl.shape[0]*imagevgl.shape[1]*imagevgl.shape[2]*imagevgl.nChannels;

  if(imagevgl.depth == CV_16U)
     ndarraySize = ndarraySize*2;
  
  gdcm::ImageChangeTransferSyntax change;
  change.SetTransferSyntax(gdcm::TransferSyntax::ExplicitVRLittleEndian);
  change.SetInput( image );
  bool b = change.Change();
  if(!b)
     std::cerr << "Could not change the Transfer Syntax" << std::endl;

  gdcm::DataElement pixeldata(gdcm::Tag(0x7fe0,0x0010));
  pixeldata.SetByteValue((char*)(imagevgl.ndarray), (uint32_t)ndarraySize);
  image.SetDataElement(pixeldata);

  gdcm::PhotometricInterpretation PI;
  PI = image.GetPhotometricInterpretation();
  if(imagevgl.nChannels == 3)
     if(PI == gdcm::PhotometricInterpretation::YBR_FULL)
        image.SetPhotometricInterpretation( gdcm::PhotometricInterpretation::RGB);
     else
        if(!(PI == gdcm::PhotometricInterpretation::RGB))
	   printf("This format is not supported");  

  gdcm::ImageWriter writer;
  writer.SetImage( change.GetOutput() );
  writer.SetFile( reader.GetFile() );
  writer.SetFileName( outfilename );
  if( !writer.Write() )
  {
    std::cerr << "Could not write image" << std::endl;
  }
  
  return 0;
}


/** Function for saving compressed DICOM images with GDCM library
  */

int vglGdcmSaveDicomCompressed(VglImage imagevgl, char* outfilename)
{
  gdcm::ImageReader reader;
  reader.SetFileName(imagevgl.filename);
  if(!reader.Read())
      std::cerr << "Could not read: " << imagevgl.filename << std::endl;
  
  // The output of gdcm::Reader is a gdcm::File
  //gdcm::File &file = reader.GetFile();

  // the dataset is the the set of element we are interested in:
  //gdcm::DataSet &ds = file.GetDataSet();

  gdcm::Image &image = reader.GetImage();
  //image.Print( std::cout );
  
  int ndarraySize = imagevgl.shape[0]*imagevgl.shape[1]*imagevgl.shape[2]*imagevgl.nChannels;

  if(imagevgl.depth == CV_16U)
     ndarraySize = ndarraySize*2;

  gdcm::ImageChangeTransferSyntax change;
  change.SetTransferSyntax( gdcm::TransferSyntax::ExplicitVRLittleEndian );
  change.SetInput( image );
  bool b = change.Change();
  if( !b )
  {
    std::cerr << "Could not change the Transfer Syntax 1" << std::endl;
    return 1;
  }

  gdcm::PhotometricInterpretation PI;
  PI = image.GetPhotometricInterpretation();
  if(imagevgl.nChannels == 3)
     if(PI == gdcm::PhotometricInterpretation::YBR_FULL)
        image.SetPhotometricInterpretation( gdcm::PhotometricInterpretation::RGB);
     else
        if(!(PI == gdcm::PhotometricInterpretation::RGB))
	   printf("This format is not supported");  
  
  gdcm::DataElement pixeldata(gdcm::Tag(0x7fe0,0x0010));
  pixeldata.SetByteValue((char*)(imagevgl.ndarray), (uint32_t)ndarraySize);
  image.SetDataElement(pixeldata);

  change.SetTransferSyntax( gdcm::TransferSyntax::JPEGBaselineProcess1 );
  //change.SetTransferSyntax( gdcm::TransferSyntax::JPEG2000Lossless );
  //change.SetTransferSyntax( gdcm::TransferSyntax::JPEGLosslessProcess14_1 );
  //change.SetTransferSyntax( image.GetTransferSyntax() );
  change.SetInput( image );
  /*bool*/ b = change.Change();
  if( !b )
  {
    std::cerr << "Could not change the Transfer Syntax" << std::endl;
    return 1;
  }

  gdcm::ImageWriter writer;
  writer.SetImage( change.GetOutput() );
  writer.SetFile( reader.GetFile() );
  writer.SetFileName( outfilename );
  if( !writer.Write() )
  {
    std::cerr << "Could not write image" << std::endl;
  }
    
  return 0;
}


#endif
