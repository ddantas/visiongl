#ifdef __TIFF__

#include <tiffio.h> 
#include <vglTiffIo.h>
#include <vglContext.h>

//malloc_usable_size
#include <malloc.h>
//memcpy, strlen
#include <string.h>

//IplImage, cvLoadImage
#ifdef __OPENCV__
  #include <opencv2/highgui/highgui_c.h>
//  #include <opencv2/imgproc/imgproc_c.h>
#else
  #include <vglOpencv.h>
#endif



/** \brief Convert depth from tiff's format to ipl's format.
  */
int convertDepthTiffToVgl(int tiffDepth)
{
  if(tiffDepth == 1)
    return IPL_DEPTH_8U;
  else if(tiffDepth == 2)
    return IPL_DEPTH_16U;
  else 
    return 0;
}

/** \brief Convert depth from ipl's format to tiff's format.
  */
int convertDepthVglToTiff(int tiffDepth)
{
  if(tiffDepth == IPL_DEPTH_8U)
    return 1;
  else if(tiffDepth == IPL_DEPTH_16U)
    return 2;
  else
    return 0;
}

void printbin(char* val, int size)
{
  int i, j;
  char buf;
  int ibuf;

  for (i = 0; i < size; i++){
    buf = val[i];
    ibuf = (char)buf;
    printf("0x");
    printf("%X", (buf & 0xF0) >> 4);
    printf("%X", (buf & 0x0F));
    printf(" = %d\n",ibuf);
  }
  for (i = 0; i < size; i++){
    buf = val[i];
    for (j = 7; j >= 0; j--){
      printf("%d", (buf & 0x80) >> 7);
      buf = buf << 1;
    }
    printf("\n");
  }
}

/* planar config   1(ok) e 2(ok)
   tiled           0(ok) e 1(ok)
   bps             8(ok) 16(ok)
   rgb separate image planes (ok)
*/


//#define macro_RasterPos2dRgb/*(w,h,iw,ih,ic)*/	\
//    (ic*w*h + ih*w + iw)

#define macro_RasterPos2dRgb/*(w,c,iw,ih,ic)*/	\
    (ih*w*c + iw*c + ic)

#define macro_RasterPos2d/*(w,iw,ih)*/          \
    (ih*w + iw)

uint32 tif_Width(TIFF* tif)
{
  uint32 w; 
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  return w;
}

uint32 tif_Height(TIFF* tif)
{
  uint32 h; 
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  return h;
}

uint32 tif_Is3d(TIFF* tif)
{
  uint32 subfileType;
  TIFFGetField(tif, TIFFTAG_SUBFILETYPE, &subfileType);
  if (subfileType)
    return 1;
  return 0;
}

uint16 tif_Layers(TIFF* tif)
{
  uint16 pageNumber, numberPages;
  TIFFGetField(tif, TIFFTAG_PAGENUMBER, &pageNumber, &numberPages);
  return numberPages;
}

tdir_t tif_DirCount(TIFF* tif)
{
    tdir_t dirCurrent = TIFFCurrentDirectory(tif);
    int dirCount = 0;
    TIFFSetDirectory(tif, 0);
    do{
      dirCount++;
    }
    while(TIFFReadDirectory(tif));
    TIFFSetDirectory(tif, dirCurrent);
    return dirCount;
}


uint16 tif_nChannels(TIFF* tif) // nChannels
{
  uint16 spp; 
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
  return spp;
}

tsize_t tif_BytesPerPixel(TIFF* tif) // depth
{
  tsize_t pixelSize; 
  uint16 bps;

  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
  switch(bps){
    case 8:
      pixelSize = 1;
      break;
    case 16:
      pixelSize = 2;
      break;
    default:
      pixelSize = 0;
      break;
  }
  return pixelSize;
}

int tif_PrintAsc(TIFF* tif, tdata_t raster, char* filename)
{

  FILE* outfile;

  char min;
  char max;
  char val;
  char pixel;

  uint32 iw, ih, ic;
  uint32 w = tif_Width(tif);
  uint32 h = tif_Height(tif);
  uint32 c = tif_nChannels(tif);
  tsize_t pixelSize = tif_BytesPerPixel(tif);

  char* rasterc;

  printf("tif_PrintAsc: w = %d, h = %d, pixelSize = %ld\n", w, h, (long int) pixelSize);

      outfile = fopen(filename, "w");
      if (!outfile)
        return 1;
      
      fprintf(outfile, "ASCII output of %s\n", filename); 

      if (raster == NULL)
        return 1;
      if (tif == NULL)
        return 1;

    rasterc = (char*) raster;
    for (ic = 0; ic < c; ic++){
      for (ih = 0; ih < h; ih++){
        for (iw = 0; iw < w; iw++){
          pixel = rasterc[pixelSize * macro_RasterPos2dRgb + (pixelSize-1)];
          min = 0; max = 4;
          if ((pixel >= min) && (pixel < max)) val = ' ';
          min = max; max = 8;
          if ((pixel >= min) && (pixel < max)) val = ',';
          min = max; max = 16;
          if ((pixel >= min) && (pixel < max)) val = '~';
          min = max; max = 32;
          if ((pixel >= min) && (pixel < max)) val = '/';
          min = max; max = 64;
          if ((pixel >= min) && (pixel < max)) val = 'o';
          min = max; max = 128;
          if ((pixel >= min) && (pixel < max)) val = 's';
          min = max; max = 192;
          if ((pixel >= min) && (pixel < max)) val = 'R';
          min = max; max = 255;
          if ((pixel >= min) && (pixel <= max)) val = '#';
          fprintf(outfile, "%c", val);       
        }
        fprintf(outfile, "\n");
      }
    }
    fclose(outfile);
    return 0;
}

tsize_t tif_NecessaryMem(TIFF* tif)
{
  uint32 w = tif_Width(tif);
  uint32 h = tif_Height(tif);
  uint32 c = tif_nChannels(tif);
  tsize_t pixelSize = tif_BytesPerPixel(tif);
  int layers = tif_Layers(tif);
  if (!tif_Is3d(tif))
    layers = 1;
  if (tif == NULL)
    return 0;
  if (c == 1)
    return w*h*pixelSize*layers;
  return w*h*pixelSize*layers*sizeof(uint32);
}

tdata_t tif_Malloc(TIFF* tif)
{
  tsize_t necessaryMem = tif_NecessaryMem(tif);

  if (necessaryMem == 0)
    return NULL;
  if (tif == NULL)
    return NULL;

  return _TIFFmalloc(necessaryMem); 
}

tdata_t tif_ReadContigStripData(TIFF* tif)
{
  tdata_t raster = tif_Malloc(tif);

  tsize_t  result;
  tsize_t  offset;
  tsize_t  stripSize = TIFFStripSize(tif);
  tstrip_t stripMax = TIFFNumberOfStrips(tif);
  tstrip_t istrip; 

  if (tif == NULL)
    return NULL;

  offset = 0;
  if (raster != NULL) { 
    for (istrip = 0; istrip < stripMax; istrip++){
      result = TIFFReadEncodedStrip (tif, istrip, ((char*)raster)+offset, stripSize);
        if (result == -1) {
          printf("Read error on input strip number %d\n", istrip);
        }
        offset += result;
    }
  }
  return raster;
}

tdata_t tif_ReadRGBData(TIFF* tif)
{
  int* buffer = (int*)tif_Malloc(tif);
  char* raster = (char*)tif_Malloc(tif);
  printf("sizeof(raster) = %ld\n", malloc_usable_size(raster));
  printf("sizeof(buffer) = %ld\n", malloc_usable_size(buffer));

  int rgba;

  tsize_t  result;
  uint16 c = tif_nChannels(tif);
  uint32 w = tif_Width(tif);
  uint32 h = tif_Height(tif);
  uint16 ic;
  uint32 iw, ih;

  if (tif == NULL)
    return NULL;

  if (buffer != NULL) { 
    printf("Reading raster rgba: w = %d, h = %d, c = %d, tif = %p, buffer = %p, raster = %p\n", w, h, c, tif, buffer, raster);
      result = TIFFReadRGBAImage(tif, w, h, (uint32*)buffer, 0);
      printf("Result = %ld\n", result);
      if (result == 0) {
          printf("Read error on input rgba image.\n");
      }
      printf("Read ok: result = %ld\n", result);
  }
  if (raster != NULL) { 
      for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
          rgba = buffer[(h-ih-1)*w+iw];
          ic = 0;
          raster[macro_RasterPos2dRgb] = TIFFGetR(rgba);
          ic = 1;
          if (c >= 2) raster[macro_RasterPos2dRgb] = TIFFGetG(rgba);
          ic = 2;
          if (c >= 3) raster[macro_RasterPos2dRgb] = TIFFGetB(rgba);
          ic = 3;
          if (c >= 4) raster[macro_RasterPos2dRgb] = TIFFGetA(rgba);
        }
      }
  }

  return (tdata_t)raster;
}

tdata_t tif_ReadData(TIFF* tif)
{
  uint16 config;
  uint16 c = tif_nChannels(tif);
  

  if (tif == NULL)
    return NULL;

  TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
  if (TIFFIsTiled(tif))
  {
    printf("is tiled\n");
    return tif_ReadRGBData(tif);
  } 
  else
  {
    if ( (config == PLANARCONFIG_CONTIG) && (c > 1) )
    {
      printf("planar config && c > 1\n");
      return tif_ReadRGBData(tif);
    }
    else
    {
      return tif_ReadContigStripData(tif);
    }
  }
}

/** Function for loading TIFF images.

    Function for loading TIFF images. Supports RGB (8 bits) and 
grayscale (8 and 16 bits) images, 2D and 3D.

  */
VglImage* vglLoadTiff(char* inFilename)
{  
  TIFF* tif;
  VglImage* img;
  uint16 pageNumber, numberPages, subfileType;

  tif = TIFFOpen(inFilename, "r");
  if (tif == NULL){
    fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, inFilename);
    return NULL;
  }
  
  int width  = tif_Width(tif);
  int height = tif_Height(tif);
  int is3d = tif_Is3d(tif);
  int layers = tif_Layers(tif);
  int depth  = tif_BytesPerPixel(tif);          // bytes per pixel
  int iplDepth = convertDepthTiffToVgl(depth);  // depth \in {IPL_DEPTH_8U, ...}
  int nChannels = tif_nChannels(tif);           // number of channels

  if (is3d)
  {
    img = vglCreate3dImage(cvSize(width,height), iplDepth, nChannels, layers, 0);
  }
  else
  {
    img = vglCreateImage(cvSize(width,height), iplDepth, nChannels);
  }

  char* imageData = img->getImageData();

  int pixelsPerFrame = img->getWidth()*img->getHeight()*img->nChannels;
  int bytesPerFrame = pixelsPerFrame*depth;
  int j = 0;
  do{
    char* buffer = (char*)tif_ReadData(tif);
    memcpy(imageData+j, buffer, bytesPerFrame);
    j += bytesPerFrame;
  }while(TIFFReadDirectory(tif));

  TIFFClose(tif); 

  vglSetContext(img, VGL_RAM_CONTEXT);

  return img;
}

/** Function for loading TIFF images.

    Function for loading TIFF images. Supports RGB (8 bits) and 
grayscale (8 and 16 bits) images, 2D and 3D.

  */
IplImage* iplLoadTiff(char* inFilename)
{  
  TIFF* tif;
  IplImage* img;
  uint16 pageNumber, numberPages, subfileType;

  tif = TIFFOpen(inFilename, "r");
  if (tif == NULL){
    fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, inFilename);
    return NULL;
  }
  
  int width  = tif_Width(tif);
  int height = tif_Height(tif);
  int is3d = tif_Is3d(tif);
  int layers = tif_Layers(tif);
  int depth  = tif_BytesPerPixel(tif);          // bytes per pixel
  int iplDepth = convertDepthTiffToVgl(depth);  // depth \in {IPL_DEPTH_8U, ...}
  int nChannels = tif_nChannels(tif);           // number of channels

  if (is3d)
  {
    fprintf(stderr, "%s:%s: Error loading file %s. Only 2D images supported. Use vglLoadTiff instead.\n", __FILE__, __FUNCTION__, inFilename);
    exit(1);
  }
  else
  {
    img = cvCreateImage(cvSize(width,height), iplDepth, nChannels);
  }

  char* imageData = img->imageData;

  int pixelsPerFrame = img->width * img->height * img->nChannels;
  int bytesPerFrame = pixelsPerFrame * depth;
  int j = 0;
  do{
    char* buffer = (char*)tif_ReadData(tif);
    memcpy(imageData+j, buffer, bytesPerFrame);
    j += bytesPerFrame;
  }while(TIFFReadDirectory(tif));

  TIFFClose(tif); 

  return img;
}

/** Function for loading TIFF images.

    Function for loading TIFF images. Supports RGB (8 bits) and 
grayscale (8 and 16 bits) images, 2D and 3D. Alternative version with simpler code.

  */
VglImage* vglLoadTiffAlt(char* inFilename)
{  
  TIFF* tif;
  VglImage* img;
  uint16 pageNumber, numberPages, subfileType;

  tif = TIFFOpen(inFilename, "r");
  if (tif == NULL){
    fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, inFilename);
    return NULL;
  }
  
  int width  = tif_Width(tif);
  int height = tif_Height(tif);
  int is3d = tif_Is3d(tif);
  int layers = tif_Layers(tif);
  int depth  = tif_BytesPerPixel(tif);          // bytes per pixel
  int iplDepth = convertDepthTiffToVgl(depth);  // depth \in {IPL_DEPTH_8U, ...}
  int nChannels = tif_nChannels(tif);           // number of channels

  if (is3d)
  {
    img = vglCreate3dImage(cvSize(width,height), iplDepth, nChannels, layers, 0);
  }
  else
  {
    img = vglCreateImage(cvSize(width,height), iplDepth, nChannels);
  }

  char* imageData = img->getImageData();
  int widthStep = img->getWidthStep();
  char* buffer = (char*) tif_Malloc(tif);

  int pixelsPerLine = img->getWidth()*img->getNChannels();
  int bytesPerLine = pixelsPerLine*depth;
  int i = 0;
  int offset = 0;
  do
  {
    /*
    for(int i = 0; i < height; i++)
    {
      printf("i = %d\n", i);
      TIFFReadScanline(tif, buffer, i);
      memcpy(imageData + offset, buffer, bytesPerLine);
      offset += widthStep;
    }
    */
    TIFFReadRGBAImage(tif, width, height, (uint32*) buffer, 0);
    memcpy(imageData + offset, buffer, widthStep*height);
    offset += widthStep + height;
  }
  while(TIFFReadDirectory(tif));

  TIFFClose(tif); 

  vglSetContext(img, VGL_RAM_CONTEXT);

  return img;
}

int vglPrintTiffInfo(char* inFilename, char* msg){
  if (msg){
    printf("====== %s:\n", msg);
  }
  else
  {
    printf("====== vglPrintTiffInfo:\n");
  }

  TIFF* tif = TIFFOpen(inFilename, "r"); 

  if (tif == NULL){
    fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, inFilename);
    return 1;
  }
  if (tif) { 
    uint32 w, h; 
    uint16 bps, spp, photo, config, pageNumber, numberPages;

    tdir_t dirCount;

    tstrip_t stripMax;
    tstrip_t istrip; 
    tsize_t stripSize;

    tsize_t npixels; 
    tsize_t pixelSize; 
    
    tdata_t raster; 
    tsize_t result, offset;
    char* rasterc;

    uint32    iw, ih; 
    int i;

    do{
      TIFFPrintDirectory(tif, stdout, 255);
    }
    while(TIFFReadDirectory(tif));

    printf("TIFFScanlineSize = %ld\n", (long int) TIFFScanlineSize(tif));
    printf("TIFFRasterScanlineSize = %ld\n", (long int) TIFFRasterScanlineSize(tif));
    printf("TIFFStripSize = %ld\n", (long int) TIFFStripSize(tif));
    printf("TIFFNumberOfStrips = %d\n", TIFFNumberOfStrips(tif));
    printf("TIFFVStripSize = %ld\n", (long int) TIFFVStripSize(tif,0));

    printf("TIFFTileRowSize = %ld\n", (long int) TIFFTileRowSize(tif));
    printf("TIFFTileSize = %ld\n", (long int) TIFFTileSize(tif));
    printf("TIFFNumberOfTiles = %d\n", TIFFNumberOfTiles(tif));
    printf("TIFFVTileSize = %ld\n", (long int) TIFFVTileSize(tif,0));

    printf("TIFFDefaultStripSize = %d\n", TIFFDefaultStripSize(tif,0));

    printf("TIFFFileno = %d\n", TIFFFileno(tif));
    printf("TIFFGetMode = %d\n", TIFFGetMode(tif));
    printf("TIFFIsTiled = %d\n", TIFFIsTiled(tif));
    printf("TIFFIsByteSwapped = %d\n", TIFFIsByteSwapped(tif));
    printf("TIFFRGBAImageOK = %d\n", TIFFRGBAImageOK(tif, (char*)"TIFFRGBAImageOK mesg\n"));



    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w); 
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h); 
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo); 
    TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
    TIFFGetField(tif, TIFFTAG_PAGENUMBER, &pageNumber, &numberPages);

    stripSize = TIFFStripSize(tif);
    stripMax = TIFFNumberOfStrips(tif);
    switch(bps){
      case 8:
        pixelSize = 1;
        break;
      case 16:
        pixelSize = 2;
        break;
      default:
        pixelSize = 0;
        break;
    }
    dirCount = tif_DirCount(tif);

    printf("size = %lu, w = %d, h = %d, spp = %d, bps = %d, pixelSize = %ld\n", sizeof(w), w, h, spp, bps, (long int) pixelSize);
    printf("Page Number = %d\n", pageNumber);
    printf("Number Pages = %d\n", numberPages);
    printf("Photometric interpretation = %d\n", photo);
    printf("Planar configuration = %d\n", config);
    printf("stripSize = %ld, stripMax = %d\n", (long int) stripSize, stripMax);
    printf("Number of directories = %d\n", dirCount);

    //printbin((char*)&w, sizeof(w));
    //printbin((char*)&h, sizeof(h));


    //raster = tif_ReadData(tif);
    //tif_PrintAsc(tif, raster, (char*)"ascimg.txt");

    TIFFClose(tif); 
  } 
  return 0;
}

/** Function for loading a stack of 3d TIFF images
  */
VglImage* vglLoad4dTiff(char* filename, int lStart, int lEnd, bool has_mipmap /*=0*/)
{
  char* tempFilename = (char*)malloc(strlen(filename) + 256);
  sprintf(tempFilename, filename, lStart);
  VglImage* tmp = vglLoadTiff(tempFilename);

  int n = lEnd-lStart+1;
  int shape[VGL_MAX_DIM+1];
  shape[0] = tmp->nChannels;
  shape[1] = tmp->getWidth();
  shape[2] = tmp->getHeight();
  shape[3] = tmp->getLength();
  shape[4] = n;

  VglImage* img = vglCreateNdImage(4, shape, tmp->depth);
  //vglPrintImageInfo(img, "4D image");

  int delta = tmp->getTotalSizeInBytes();
  int offset = 0;
  vglReleaseImage(&tmp);
  for(int i = lStart; i <= lEnd; i++)
  {
    sprintf(tempFilename, filename, i);
    printf("filename[%d] = %s\n", i, tempFilename);
    VglImage* tmp = vglLoadTiff(tempFilename);
    memcpy(img->getImageData() + offset, tmp->getImageData(), delta);
    offset += delta;
  }

  vglSetContext(img, VGL_RAM_CONTEXT);

  return img;
}

/** Function for saving 2D and 3D TIFF images
  */
int vglSaveTiff(char* outFilename, VglImage* image)
{
  vglCheckContext(image, VGL_RAM_CONTEXT);

  TIFF *out = TIFFOpen(outFilename, "w");
  char* buff = image->getImageData();

  //int b = image->getBitsPerSample();
  int c = image->getNChannels();
  int widthStep = image->getWidthStep();

  for(int z = 0; z < image->getLength(); z++)
  {
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, image->getWidth());
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, image->getHeight());
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, image->getBitsPerSample());
    if (image->nChannels == 1)
      TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    else
      TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, image->nChannels);
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
   
    TIFFSetField(out, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
    TIFFSetField(out, TIFFTAG_PAGENUMBER, z, image->getLength());

    for(int y = 0; y < image->getHeight(); y++)
    {
      TIFFWriteScanline(out, &buff[ (  (z*widthStep*image->getHeight()) + (y*widthStep)  )], y, 0);
    }
    
    TIFFWriteDirectory(out);
  }
 
  TIFFClose(out);
}

/** Function for saving 2D TIFF images
  */
int iplSaveTiff(char* outFilename, IplImage* image)
{
  TIFF *out = TIFFOpen(outFilename, "w");
  char* buff = image->imageData;

  int b = convertDepthVglToTiff(image->depth); //bytes per sample
  int c = image->nChannels;
  int widthStep = image->widthStep;
  int w = image->width;
  int h = image->height;

  TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
  TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
  TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, b * 8);
  if (image->nChannels == 1)
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
  else
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, c);
  TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

  TIFFSetField(out, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
  TIFFSetField(out, TIFFTAG_PAGENUMBER, 0, 1);

  for(int y = 0; y < image->height; y++)
  {
    TIFFWriteScanline(out, &buff[ (y * widthStep)  ], y, 0);
  }
    
  TIFFWriteDirectory(out);

  TIFFClose(out);
  return 0;
}

int vglSave4dTiff(char* filename, VglImage* image, int lStart, int lEnd)
{
  vglCheckContext(image, VGL_RAM_CONTEXT);
  if ( (image->nChannels != 1) && (image->nChannels != 3) )
  {
    fprintf(stderr, "%s: %s: Error: image has %d channels but only 1 or 3 channels supported. Use vglImage4to3Channels function before saving\n", __FILE__, __FUNCTION__, image->nChannels);
    return 1;
  }
  char* temp_filename = (char*)malloc(strlen(filename)+256);
  int c = 0;
  for(int i = lStart; i <= lEnd; i++)
  {
    VglImage* temp_image = vglCreate3dImage(cvSize(image->getWidth(), image->getHeight()), image->depth, image->nChannels, image->getLength());
    temp_image->ndarray = (char*)malloc(temp_image->getTotalSizeInBytes());
    memcpy((char*)temp_image->ndarray,((char*)image->ndarray)+c,temp_image->getTotalSizeInBytes());
    sprintf(temp_filename, filename, i);
    vglSaveTiff(temp_filename, temp_image);
    c += temp_image->getTotalSizeInBytes();
    vglReleaseImage(&temp_image);
  }

  return 0;
}      


#endif
