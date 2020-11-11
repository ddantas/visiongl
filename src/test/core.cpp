
#include "vglShape.h"

#include "vglTest.h"

void testVglShape()
{
  //Arrange
  int w = 100;
  int h = 50;
  int d3 = 30;
  int d4 = 20;
  int d5 = 10;
  int shape[VGL_MAX_DIM+1] = {1, w, h, d3, d4, d5};
  int bps[] = {1, 8, 16, 32, 64};
  int iBpsMax = 4;
  char msg[255];

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
    Test VisionGL core:\n\
\n\
        test_core\n\
\n\
";

  testVglShape();

  return 0;
}
