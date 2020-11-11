
#include "vglImage.h"

#include "cl2cpp_shaders.h"
#include "cl2cpp_BIN.h"
#include "vglClImage.h"
#include "vglClFunctions.h"
#include "vglContext.h"
#include "iplImage.h"

#include "vglTest.h"

void testVglCl(VglImage* im2d, VglImage* im3d)
{
  char msg[255];
  bool eq;
  int eqi;
  char* imageData;
  char p;
  int c;

  //Arrange
  VglImage* im2d_2 = vglCreateImage(im2d);
  vglClCopy(im2d, im2d_2);

  //Act
  eq = vglClEqual(im2d, im2d_2);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglEqual: comparison of 2 equal images");
  ASSERT_EQ_I(1, eqi, msg);

  //////////////////// 2D
    
  //Arrange
  vglClUploadForce(im2d);

  //Act
  eq = vglClEqual(im2d, im2d_2);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglEqual: comparison of 2 equal images");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglCheckContext(im2d_2, VGL_RAM_CONTEXT);
  vglClUploadForce(im2d_2);

  //Act
  eq = vglClEqual(im2d, im2d_2);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglEqual: comparison of 2 equal images");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglCheckContext(im2d_2, VGL_RAM_CONTEXT);
  imageData = im2d_2->getImageData();
  c = 100;
  p = imageData[c];
  printf("p = %d\n", imageData[c]);
  imageData[c]++;
  printf("p = %d\n", imageData[c]);
  vglClUploadForce(im2d_2);

  //Act
  eq = vglClEqual(im2d, im2d_2);
  printf("eq = %d\n", eq);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglEqual: comparison of 2 different images");
  ASSERT_NEQ_I(1, eqi, msg);

  //////////////////// 2D BIN
    
  int x0 = 0;
  int y0 = 0;
  int x1 = im2d->getWidth();
  int y1 = im2d->getHeight();
  float thresh = 0.282;
  VglImage* vglThresh = vglCreateImage(cvSize(im2d->getWidth(), im2d->getHeight()), IPL_DEPTH_1U, 1);
  VglImage* vglSwap   = vglCreateImage(cvSize(im2d->getWidth(), im2d->getHeight()), IPL_DEPTH_1U, 1);
  VglImage* vglNot =    vglCreateImage(cvSize(im2d->getWidth(), im2d->getHeight()), IPL_DEPTH_1U, 1);
  VglImage* vglRoi =    vglCreateImage(cvSize(im2d->getWidth(), im2d->getHeight()), IPL_DEPTH_1U, 1);
  VglImage* vglResult = vglCreateImage(cvSize(im2d->getWidth(), im2d->getHeight()), IPL_DEPTH_1U, 1);

  vglClBinThreshold(im2d, vglThresh, thresh);
  vglClBinSwap(vglThresh, vglSwap);
  vglCheckContext(vglSwap, VGL_RAM_CONTEXT);
  iplSavePgm("/tmp/testcl_thresh.pbm", vglSwap->ipl);

  vglClBinRoi(vglRoi, x0, y0, x1, y1);
  vglClBinSwap(vglRoi, vglSwap);
  vglCheckContext(vglSwap, VGL_RAM_CONTEXT);
  iplSavePgm("/tmp/testcl_roi.pbm", vglSwap->ipl);
  
  vglClBinNot(vglThresh, vglNot);
  vglClBinSwap(vglNot, vglSwap);
  vglCheckContext(vglSwap, VGL_RAM_CONTEXT);
  iplSavePgm("/tmp/testcl_not.pbm", vglSwap->ipl);

  vglCheckContext(vglThresh, VGL_RAM_CONTEXT);
  iplSavePgm("/tmp/testcl_swap.pbm", vglThresh->ipl);


  ////////////////////
    
  //Arrange
  vglClBinMax(vglRoi, vglNot, vglResult);

  //Act
  eq = vglClBinEqual(vglResult, vglRoi);
  printf("eq = %d\n", eq);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglBinMax: test if A | ~A == 1");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglClBinCopy(vglNot, vglResult);
  vglClBinMax(vglRoi, vglResult, vglResult);

  //Act
  eq = vglClBinEqual(vglResult, vglRoi);
  printf("eq = %d\n", eq);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglBinMax: test if A | ~A == 1 (on place)");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglClBinMin(vglRoi, vglNot, vglResult);

  //Act
  eq = vglClBinEqual(vglResult, vglNot);
  printf("eq = %d\n", eq);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglBinMin: test if 1 & ~A == ~A");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglClBinCopy(vglNot, vglResult);
  vglClBinMin(vglRoi, vglResult, vglResult);

  //Act
  eq = vglClBinEqual(vglResult, vglNot);
  printf("eq = %d\n", eq);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglBinMin: test if 1 & ~A == ~A (on place)");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglClBinSub(vglRoi, vglThresh, vglResult);

  //Act
  eq = vglClBinEqual(vglResult, vglNot);
  printf("eq = %d\n", eq);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglBinSub: test if 1 - A == ~A");
  ASSERT_EQ_I(1, eqi, msg);


  //////////////////// 3D
    
  //Arrange
  VglImage* im3d_2 = vglCreateImage(im3d);
  vglCl3dCopy(im3d, im3d_2);

  //Act
  eq = vglCl3dEqual(im3d, im3d_2);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglEqual: comparison of 2 equal 3d images");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglClUploadForce(im3d);

  //Act
  eq = vglCl3dEqual(im3d, im3d_2);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglEqual: comparison of 2 equal 3d images");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglCheckContext(im3d_2, VGL_RAM_CONTEXT);
  vglClUploadForce(im3d_2);

  //Act
  eq = vglCl3dEqual(im3d, im3d_2);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglEqual: comparison of 2 equal 3d images");
  ASSERT_EQ_I(1, eqi, msg);

  ////////////////////
    
  //Arrange
  vglDownload(im3d_2);
  imageData = im3d_2->getImageData();
  c = 100;
  p = imageData[c];
  printf("p = %d\n", imageData[c]);
  imageData[c]++;
  printf("p = %d\n", imageData[c]);
  vglClUploadForce(im3d_2);

  //Act
  eq = vglCl3dEqual(im3d, im3d_2);
  printf("eq = %d\n", eq);
  eqi = 1 * eq;

  //Assert
  sprintf(msg, "vglEqual: comparison of 2 different 3d images");
  ASSERT_NEQ_I(1, eqi, msg);

  

  exit(0);

  int w = 100;
  int h = 50;
  int d3 = 30;
  int d4 = 20;
  int d5 = 10;
  int shape[VGL_MAX_DIM+1] = {1, w, h, d3, d4, d5};
  int bps[] = {1, 8, 16, 32, 64};
  int iBpsMax = 4;

  ASSERT_SECTION_START((char*) "VglShape");
  
  for (int nCh = 1; nCh <= 4; nCh++)
  {
    //Act
    VglShape vglShape2d = VglShape(nCh, w, h);

    vglShape2d.print();

    //Assert
    sprintf(msg, "VglShape 2d, nCh = %d: size", nCh);
    ASSERT_EQ_I(w*h*nCh, vglShape2d.getSize(), msg);

    //Assert
    sprintf(msg, "VglShape 2d, nCh = %d: Npixels", nCh);
    ASSERT_EQ_I(w*h,     vglShape2d.getNpixels(), msg);

    //Assert
    sprintf(msg, "VglShape 2d, nCh = %d: Ndim", nCh);
    ASSERT_EQ_I(2,       vglShape2d.getNdim(), msg);
  }

  for (int iBps = 1; iBps <= iBpsMax; iBps++)
  {
    int Bytes = bps[iBps] / 8;
    int nFrames = 1;
    for (int nDim = 2; nDim <= 5; nDim++)
    {
      if (nDim > 2)
      {
        nFrames *= shape[nDim];
      }
      for (int nCh = 1; nCh <= 4; nCh++)
      {
        //Act
	shape[0] = nCh;
        VglShape vglShapeNd = VglShape(shape, nDim, bps[iBps]);
        int getNFrames = vglShapeNd.getNFrames();

        vglShapeNd.print();
	
        //Assert
        sprintf(msg, "VglShape %dd, bps = %d, nCh = %d: NFrames", nDim, bps[iBps], nCh);
        ASSERT_EQ_I(nFrames, getNFrames, msg);

        //Assert
        sprintf(msg, "VglShape %dd, bps = %d, nCh = %d: size", nDim, bps[iBps], nCh);
        ASSERT_EQ_I(w*h*nFrames*nCh*Bytes, vglShapeNd.getSize(), msg);

        //Assert
        sprintf(msg, "VglShape %dd, bps = %d, nCh = %d: Npixels", nDim, bps[iBps], nCh);
        ASSERT_EQ_I(w*h*nFrames,           vglShapeNd.getNpixels(), msg);

        //Assert
        sprintf(msg, "VglShape %dd, bps = %d, nCh = %d: Ndim", nDim, bps[iBps], nCh);
        ASSERT_EQ_I(nDim,                  vglShapeNd.getNdim(), msg);
      }
    }
  }

  sprintf(msg, "VGL_PACK_SIZE_BITS  = %d", VGL_PACK_SIZE_BITS);
  ASSERT_PRINT_MSG(msg);
  sprintf(msg, "VGL_PACK_SIZE_BYTES = %d", VGL_PACK_SIZE_BYTES);
  ASSERT_PRINT_MSG(msg);
  
  int iBps = 0;
  int nCh = 1;
  shape[0] = nCh;
  {
    int nFrames = 1;
    for (int nDim = 2; nDim <= 5; nDim++)
    {
      if (nDim > 2)
      {
        nFrames *= shape[nDim];
      }
      //Act
      VglShape vglShapeNdBin = VglShape(shape, nDim, bps[iBps]);
      int getNFrames = vglShapeNdBin.getNFrames();
      int widthStep = ( (w - 1) / VGL_PACK_SIZE_BITS + 1 ) * VGL_PACK_SIZE_BYTES;

      vglShapeNdBin.print();

      //Assert
      sprintf(msg, "VglShape %dd, bps = %d, nCh = %d: NFrames", nDim, bps[iBps], nCh);
      ASSERT_EQ_I(nFrames, getNFrames, msg);

      //Assert
      sprintf(msg, "VglShape %dd, bps = %d, nCh = %d: size", nDim, bps[iBps], nCh);
      ASSERT_EQ_I(widthStep*h*nFrames,   vglShapeNdBin.getSize(), msg);

      //Assert
      sprintf(msg, "VglShape %dd, bps = %d, nCh = %d: Npixels", nDim, bps[iBps], nCh);
      ASSERT_EQ_I(w*h*nFrames,           vglShapeNdBin.getNpixels(), msg);

      //Assert
      sprintf(msg, "VglShape %dd, bps = %d, nCh = %d: Ndim", nDim, bps[iBps], nCh);
      ASSERT_EQ_I(nDim,                  vglShapeNdBin.getNdim(), msg);
    }
  }

  ASSERT_SECTION_END();

}


int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
    Test VisionGL CL functions. Usage is as follows:\n\
\n\
test_cl <input file>\n\
\n\
where input file is an image file.\n\
\n\
";

  if (argc < 2)
  {
    printf("%s", usage);
    exit(1);
  }

  char *inFilename = argv[1]; // name of the input file
  VglImage* im2d = vglLoadImage((char*) inFilename);
  int imgIFirst = 0;
  int imgILast =  2;
  VglImage* im3d = vglLoad3dImage((char*) inFilename, imgIFirst, imgILast);

  testVglCl(im2d, im3d);

  return 0;
}
