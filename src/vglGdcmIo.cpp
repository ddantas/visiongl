#ifdef __GDCM__

#include <vglGdcmIo.h>
#include <gdcmImageReader.h>
#include <gdcmImage.h>
#include <gdcmWriter.h>
#include <gdcmAttribute.h>
#include <gdcmImageWriter.h>
#include <gdcmImageChangeTransferSyntax.h>

#include <iostream>
#include <fstream>


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
  image.Print( std::cout );
  
  gdcm::PixelFormat pixelformat = image.GetPixelFormat();
  VglImage imagevgl;
  
  imagevgl.shape[0] = image.GetColumns(); // width/columns
  imagevgl.shape[1] = image.GetRows(); // height/rows
  imagevgl.shape[2] = (image.GetDimensions())[2]; // number of frames
  
  int ndarraySize = imagevgl.shape[0]*imagevgl.shape[1]*imagevgl.shape[2];
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
       
  imagevgl.nChannels = pixelformat.GetSamplesPerPixel(); // number of channels of image 
  imagevgl.ndim = image.GetNumberOfDimensions(); // number of dimensions

  /*printf("\n\nColumns: %d\nRows: %d\nFrames: %d\nDepth: %d\nChannels: %d\nndim: %d\n\n", imagevgl.shape[0], imagevgl.shape[1], imagevgl.shape[2], imagevgl.depth, imagevgl.nChannels, imagevgl.ndim);*/

  return imagevgl;
}


/** Function for saving uncompressed DICOM images with GDCM library
  */

int vglGdcmSaveDicom(VglImage imagevgl, char* filename, char* outfilename)
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
  image.Print( std::cout );

  int ndarraySize = imagevgl.shape[0]*imagevgl.shape[1]*imagevgl.shape[2];

  gdcm::DataElement pixeldata(gdcm::Tag(0x7fe0,0x0010));
  pixeldata.SetByteValue((char*)(imagevgl.ndarray), (uint32_t)ndarraySize);
  image.SetDataElement(pixeldata);
  
  gdcm::ImageChangeTransferSyntax change;
  change.SetTransferSyntax(gdcm::TransferSyntax::ExplicitVRLittleEndian);
  change.SetInput( image );
  bool b = change.Change();
  if(!b)
     std::cerr << "Could not change the Transfer Syntax" << std::endl;
  
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
