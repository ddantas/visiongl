#include <vglDeconv.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


/* Algorithm based on the imagej plugin, Colour Deconvolution, by 
Gabriel Landini. The plugin is available in: 
http://fiji.sc/Colour_Deconvolution. Accessed in: 22.08.2014.
 */

/** Function to convert image in BGR format to RGB format
 */
void swapChannels(VglImage *src, int a, int b)
{
  unsigned char r;
  int nPixels = src->shape[0]*src->shape[1];
  for(int j = 0; j < nPixels; j++)
  { 
    r = ((unsigned char *)src->ndarray)[j*3+a];
    ((unsigned char *)src->ndarray)[j*3+a] = ((unsigned char *)src->ndarray)[j*3+b];
    ((unsigned char *)src->ndarray)[j*3+b] = r;
  } 
}


/** Function to convert image in RGB format to BGR format
 */
void convertRGBToBGR(VglImage *src)
{
  swapChannels(src, 0, 2);
}

/** Function to convert image in BGR format to RGB format
 */

void convertBGRToRGB(VglImage *src)
{
  swapChannels(src, 0, 2);
}

/** Function to invert matrix 3x3
 */

int invertMatrix3x3(double *mNorm, double *mInv)
{
  
  double mNormal[3][3], mInverse[3][3], mTranspose[3][3];
  double value[3];
  double detmNormal;
  
  int i = 0;
  for(int row = 0; row < 3; row++)
    for(int col = 0; col <3; col++)
    {
      mNormal[row][col] = mNorm[i];
      i++;
    }
 
  // Calculation of the determinant
  value[0] = mNormal[0][0]*((mNormal[1][1]*mNormal[2][2])-(mNormal[1][2]*mNormal[2][1]));
  value[1] = (-1*mNormal[0][1])*((mNormal[1][0]*mNormal[2][2])-(mNormal[1][2]*mNormal[2][0]));
  value[2] = mNormal[0][2]*((mNormal[1][0]*mNormal[2][1])-(mNormal[1][1]*mNormal[2][0]));
  detmNormal = value[0]+value[1]+value[2];
  
  printf("\nDeterminant = %f\n", detmNormal);

  if(detmNormal == 0)
  {
    printf("\nMatrix not invertible\n");
    return 1;
  }


  // Composition of the Transposed Matrix
  for(int row = 0; row < 3; row++)
    for(int col = 0; col <3; col++)
      	mTranspose[row][col] = mNormal[col][row];


  // Show Transposed Matrix
  printf("\nTransposed Matrix:\n");
  for(int row = 0; row < 3; row++)
  {
    for(int col = 0; col <3; col++)
      printf("%f ", mTranspose[row][col]);
    printf("\n");
  }


  // Composition of the Adjunct Matrix
  double mAdjunct[3][3];
  //row 0
  mAdjunct[0][0] = (mTranspose[1][1]*mTranspose[2][2])-(mTranspose[1][2]*mTranspose[2][1]);
  mAdjunct[0][1] = -1*((mTranspose[1][0]*mTranspose[2][2])-(mTranspose[1][2]*mTranspose[2][0]));
  mAdjunct[0][2] = (mTranspose[1][0]*mTranspose[2][1])-(mTranspose[1][1]*mTranspose[2][0]);
  //row 1
  mAdjunct[1][0] = -1*((mTranspose[0][1]*mTranspose[2][2])-(mTranspose[0][2]*mTranspose[2][1]));
  mAdjunct[1][1] = (mTranspose[0][0]*mTranspose[2][2])-(mTranspose[0][2]*mTranspose[2][0]);
  mAdjunct[1][2] = -1*((mTranspose[0][0]*mTranspose[2][1])-(mTranspose[0][1]*mTranspose[2][0]));
  //row 2
  mAdjunct[2][0] = (mTranspose[0][1]*mTranspose[1][2])-(mTranspose[0][2]*mTranspose[1][1]);
  mAdjunct[2][1] = -1*((mTranspose[0][0]*mTranspose[1][2])-(mTranspose[0][2]*mTranspose[1][0]));
  mAdjunct[2][2] = (mTranspose[0][0]*mTranspose[1][1])-(mTranspose[0][1]*mTranspose[1][0]);
  
  // Show Adjunct Matrix     
  printf("\nAdjunct Matrix:\n");
  for(int row = 0; row < 3; row++)
  {
    for(int col = 0; col <3; col++)
      printf("%f ", mAdjunct[row][col]);
    printf("\n");
  }

  // Composition of the Inverse Matrix
  for(int row = 0; row < 3; row++)
    for(int col = 0; col <3; col++)
      mInverse[row][col] = (1/detmNormal)*mAdjunct[row][col];

  i = 0;
  for(int row = 0; row < 3; row++)
    for(int col = 0; col <3; col++)
    {
      mInv[i] = mInverse[row][col];
      i++;
    }
  
  // Show Inverse Matrix 
  printf("\nInverse Matrix:\n");
  for(int i = 0; i < 9; i++)
  {  
    printf("%f ", mInv[i]);
    if((i != 0) & (((i+1) % 3) == 0))
      printf("\n");
  }

  printf("\n");
  return 0;
}


/** Function to normalize matrix 3x3
 */

int normalizeMatrix3x3(double *mIni, double *mNorm)
{ 
  double mInitial[3][3], mNormal[3][3];
  double norm;

  int i = 0;
  for(int row = 0; row < 3; row++)
    for(int col = 0; col < 3; col++)
    {
      mInitial[row][col] = mIni[i];
      mNormal[row][col] = 0.0;
      i++;
    }

  // Composition of the Normalized Matrix
  for(int col = 0; col < 3; col++)
  {
    norm = 0;
    for(int row = 0; row < 3; row++)
      norm += pow(mInitial[row][col], 2.0);
    if(norm != 0.0)
      for(int row = 0; row < 3; row++)
	mNormal[row][col] = mInitial[row][col]/sqrt(norm);
  }    
 
  // 3rd value is unspecified
  if((mNormal[0][2] == 0.0) && (mNormal[1][2] == 0.0) && (mNormal[2][2] == 0.0))
  {
    if((pow(mNormal[0][0], 2.0) + pow(mNormal[0][1], 2.0)) > 1)
    {  
      printf("Color 3 has a negative R component.");
      mNormal[0][2] = 0.0;
    }
    else
      mNormal[0][2] = sqrt(1.0 - (pow(mNormal[0][0], 2.0) - (pow(mNormal[0][1], 2.0))));

    if((pow(mNormal[1][0], 2.0) + pow(mNormal[1][1], 2.0)) > 1)
    {  
      printf("Color 3 has a negative G component.");
      mNormal[1][2] = 0.0;
    }
    else
      mNormal[1][2] = sqrt(1.0 - (pow(mNormal[1][0], 2.0) - (pow(mNormal[1][1], 2.0))));

    if((pow(mNormal[2][0], 2.0) + pow(mNormal[2][1], 2.0)) > 1)
    {  
      printf("Color 3 has a negative B component.");
      mNormal[2][2] = 0.0;
    }
    else
      mNormal[2][2] = sqrt(1.0 - (pow(mNormal[2][0], 2.0) - (pow(mNormal[2][1], 2.0))));
  }

  norm = sqrt(pow(mNormal[0][2], 2.0) + pow(mNormal[1][2], 2.0) + pow(mNormal[2][2], 2.0));
  
  for(i = 0; i < 3; i++)
    mNormal[i][2] = mNormal[i][2]/norm;
    
  for(int row = 0; row < 3; row++)
    for(int col = 0; col < 3; col++)
      if(mNormal[row][col] == 0.0)
	mNormal[row][col] = 0.001;


  i = 0;
  for(int row = 0; row < 3; row++)
    for(int col = 0; col <3; col++)
    {
      mNorm[i] = mNormal[row][col];
      i++;
    }

  // Show Initial Matrix
  printf("\nInitial Matrix:\n");
  for(int i = 0; i < 9; i++)
  {  
    printf("%f ", mIni[i]);
    if((i != 0) & (((i+1) % 3) == 0))
      printf("\n");
  }

  // Show Normalized Matrix
  printf("\nNormalized Matrix:\n");
  for(int i = 0; i < 9; i++)
  {  
    printf("m[%d] = %f ", i, mNorm[i]);
    if((i != 0) & (((i+1) % 3) == 0))
      printf("\n");
  }
  
  return 0;
}


/** Function to save three RGB images, one for each deconvolved color
 */
void vglSaveColorDeconv(VglImage *imagevgl, double *mInitial, char *outFilename)
{
  double *mNormal = (double *) malloc(9*sizeof(double));
  normalizeMatrix3x3(mInitial, mNormal);

  convertBGRToRGB(imagevgl);
  
  VglImage *newimagevgl[3];
  for(int i = 0; i < 3; i++)
    newimagevgl[i] = vglCreate3dImage(cvSize(imagevgl->shape[0], imagevgl->shape[1]), imagevgl->depth, imagevgl->nChannels, imagevgl->shape[2]);

  int nPixels = imagevgl->shape[0]*imagevgl->shape[1];
  for(int j = 0; j < nPixels; j++)
    for(int i = 0; i < 3; i++)
    { 
      unsigned char gray = ((unsigned char *)imagevgl->ndarray)[j*3+i];
      double rLut = 255.0 - ((255.0 - ((double)gray)) * mNormal[i]);
      double gLut = 255.0 - ((255.0 - ((double)gray)) * mNormal[i+3*1]);
      double bLut = 255.0 - ((255.0 - ((double)gray)) * mNormal[i+3*2]);
      ((unsigned char *)newimagevgl[i]->ndarray)[j*3] = ((unsigned char)rLut);
      ((unsigned char *)newimagevgl[i]->ndarray)[j*3+1] = ((unsigned char)gLut);
      ((unsigned char *)newimagevgl[i]->ndarray)[j*3+2] = ((unsigned char)bLut);
    }

  int lStart = 0;
  int lEnd = 0;
  for(int i = 0; i < 3; i++)
  {
    convertRGBToBGR(newimagevgl[i]);
    char outname[1024];
    sprintf(outname, outFilename, i);
    vglSave3dImage(newimagevgl[i], outname, lStart, lEnd);
  }  
}

/** Function to deconvolve image colors
 */

VglImage* vglColorDeconv(VglImage *imagevgl, double *mInitial)
{
  
  double *mNormal = (double *) malloc(9*sizeof(double));
  double *mInverse = (double *) malloc(9*sizeof(double));
  double log255 = log(255.0);
  
  normalizeMatrix3x3(mInitial, mNormal);
  invertMatrix3x3(mNormal, mInverse);
  convertBGRToRGB(imagevgl);

  //vglPrintImageInfo(imagevgl);

  // Creation of structure for the new image
  VglImage *newimagevgl;
  newimagevgl = vglCreate3dImage(cvSize(imagevgl->shape[0], imagevgl->shape[1]), imagevgl->depth, imagevgl->nChannels, imagevgl->shape[2]);

  // Composition of the new image
  int nPixels = imagevgl->shape[0]*imagevgl->shape[1];
  for(int j = 0; j < nPixels; j++)
  {
    // log transform the RGB data
    unsigned char r = ((unsigned char *)imagevgl->ndarray)[j*3];
    unsigned char g = ((unsigned char *)imagevgl->ndarray)[j*3+1];
    unsigned char b = ((unsigned char *)imagevgl->ndarray)[j*3+2];
    double rLog = -((255.0 * log(((double)r+1)/255.0))/log255);
    double gLog = -((255.0 * log(((double)g+1)/255.0))/log255);
    double bLog = -((255.0 * log(((double)b+1)/255.0))/log255);
   
    for(int i = 0; i < 3; i++)
    { 
      double rScaled = rLog * mInverse[i*3];
      double gScaled = gLog * mInverse[i*3+1];
      double bScaled = bLog * mInverse[i*3+2];
      double output = exp(-((rScaled + gScaled + bScaled) - 255.0) * log255 / 255.0);
      if(output > 255) 
	output = 255;
      ((unsigned char *)newimagevgl->ndarray)[j*3+i] = (unsigned char)output;
    }
  }

  convertRGBToBGR(newimagevgl);
  return newimagevgl; 
}

