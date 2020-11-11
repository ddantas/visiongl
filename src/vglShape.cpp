
#include <vglConst.h>
#include <vglShape.h>

#ifdef __OPENCL__
#include <vglClShape.h>
#endif

//printf
#include <stdio.h>
//exit
#include <cstdlib>

/** /brief Common code for shape construction.

    Common code for shape construction. Function called from all VglShape
    constructors. Ideally, should be a constructor called from other constructors,
    but C++03 does not support it.

    Receives three parameters:
    shape: array with size of each dimension, including number of channels in position 0.
    ndim: number of dimensions.
    bps: bits per sample.
 */
void VglShape::vglCreateShape(int* shape, int ndim, int bps /*= 8*/)
{
  this->ndim = ndim;
  this->bps = bps;
  this->size = 1;
  if ( (bps == 1) && (shape[0] != 1) )
  {
    fprintf(stderr,"%s, %s, Error: Image with 1 bps and nChannels = %d != 1\n", __FILE__, __FUNCTION__, shape[0]);
    exit(1);
  }
  int maxi = ndim;
  int c = shape[VGL_SHAPE_NCHANNELS];
  int w = shape[VGL_SHAPE_WIDTH];
  if (ndim == 1)
  {
    maxi = 2;
  }
  for (int i = 0; i <= VGL_MAX_DIM; i++)
  {
    if (i <= maxi)
    {
      this->shape[i] = shape[i];
      if(i == 0)
      {
        this->offset[i] = 1;
      }
      else if (i == 2)
      {
        this->offset[i] = findWidthStep(bps, w, c);
      }
      else
      {
        this->offset[i] = shape[i - 1] * this->offset[i - 1];
      }
    }
    else
    {
      this->shape[i]  = 1;
      this->offset[i] = 0;
    }
  }
  this->size *= this->shape[maxi] * this->offset[maxi];
}

/** /brief Constructs shape from another shape.

    Receives one parameter:
    vglShape: shape to be copied.
 */
VglShape::VglShape(VglShape* vglShape)
{
  this->vglCreateShape(vglShape->shape, vglShape->ndim, vglShape->bps);
}
  
/** /brief Generic shape constructor.

    Receives three parameters:
    shape: array with size of each dimension, including number of channels in position 0.
    ndim: number of dimensions.
    bps: bits per sample. Default value is 8.
 */
VglShape::VglShape(int* shape, int ndim, int bps /*= 8*/)
{
  this->vglCreateShape(shape, ndim, bps);
}
  
/** /brief 1D shape constructor.

    1D shapes are internally treated as 2D images. Receives two parameters:
    w: width.
    h: height.
 */
VglShape::VglShape(int w, int h)
{
  int shape[VGL_MAX_DIM+1];
  int ndim = 2;
  shape[0] = 1;
  shape[1] = w;
  shape[2] = h;
  this->vglCreateShape(shape, ndim);
}

/** /brief 2D shape constructor.

    Receives three parameters:
    nChanels: number of channels.
    w: width.
    h: height.
 */
VglShape::VglShape(int nChannels, int w, int h)
{
  int shape[VGL_MAX_DIM+1];
  int ndim = 2;
  shape[0] = nChannels;
  shape[1] = w;
  shape[2] = h;
  this->vglCreateShape(shape, ndim);
}

/** /brief 3D shape constructor.

    Receives four parameters:
    nChanels: number of channels.
    w: width.
    h: height.
    d3: number of frames.
 */
VglShape::VglShape(int nChannels, int w, int h, int d3)
{
  int shape[VGL_MAX_DIM+1];
  int ndim = 3;
  shape[0] = nChannels;
  shape[1] = w;
  shape[2] = h;
  shape[3] = d3;
  this->vglCreateShape(shape, ndim);
}


/** /brief Shape destructor.

    Shape destructor.
 */
VglShape::~VglShape()
{

}

/** /brief Get index from coordinate array.

    Get index from coordinate array. Calculates value of index by multiplying coordinate values 
    by respective offset, and summing up the results.
 */
int VglShape::getIndexFromCoord(int* coord)
{
  int result = 0;
  for (int d = 0; d <= ndim; d++)
  {
    result += this->offset[d] * coord[d];
  }
  return result;
}

/** /brief Get coordinate array from index.

    Get coordinate array from index. Calculates value of coordinates by dividing index 
    by respective offset.
 */
void VglShape::getCoordFromIndex(int index, int* coord)
{
  int ndim = this->getNdim();
  int* shape = this->getShape();
  int* offset = this->getOffset();
  int ires = index;
  int idim;
  for(int d = ndim; d >= 0; d--)
  {
    idim = ires / offset[d];
    ires = ires - idim * offset[d];
    coord[d] = idim;
  }
}

/** /brief Get size.

    Get total size of shape. Is the product of all shape array positions, including
    number of channels. Total size of shape in bytes is getSize()*sizeof(type).
 */
int VglShape::getSize()
{
  return this->size;
}

/** /brief Get number of pixels.

    Get number of pixels of shape. Is the product of all shape array positions, 
    not including number of channels.
 */
int VglShape::getNpixels()
{
  return this->size / this->shape[VGL_SHAPE_NCHANNELS];
}

/** /brief Get number of dimensions.

    Get number of dimension of shape. Shape array valid positions is this->getNdim()+1
    as position 0 contains number of channels, which is not considered a dimension.
 */
int VglShape::getNdim()
{
  return this->ndim;
}

/** /brief Get bits per sample.

    Get number of bits per sample. Valid values are 1, 8 and 16.
 */
int VglShape::getBps()
{
  return this->bps;
}

/** /brief Get shape array.

    Get shape array, with size of each dimension and number of channels in position 0.
 */
int* VglShape::getShape()
{
  return this->shape;
}

/** /brief Get offset array.

    Get offset array.
 */
int* VglShape::getOffset()
{
  return this->offset;
}

void VglShape::printArray(int* arr, int ndim)
{
  printf("[");
  for(int i = 0; i <= VGL_MAX_DIM; i++)
  {
    printf("%5d", arr[i]);
    if(i == ndim)
    {
      printf("]");
    }
    else if(i < VGL_MAX_DIM)
    {
      printf(",");
    }
  }
}

void VglShape::printInfo()
{
  printf("ndim    = %d\n", this->getNdim());
  printf("shape   = ");
  this->printArray(this->getShape(), this->getNdim());
  printf("\n");
  printf("offset  = ");
  this->printArray(this->getOffset(), this->getNdim());
  printf("\n");
  printf("size    = %d\n", this->getSize());
  printf("npixels = %d\n", this->getNpixels());
  printf("bps     = %d\n", this->getBps());
}


/** Print information about shape.

    Print dimensions, offsets, total size and number of dimensions.

 */
void VglShape::print(char* msg)
{
  if (msg){
    printf("====== %s:\n", msg);
  }
  else
  {
    printf("====== VglShape->print():\n");
  }
  this->printInfo();
}

int VglShape::getNChannels()
{
  return this->shape[VGL_SHAPE_NCHANNELS];
}

int VglShape::getWidth()
{
  if (this->ndim == 1)
  {
    return this->shape[VGL_SHAPE_WIDTH] * this->shape[VGL_SHAPE_HEIGHT];
  }
  return this->shape[VGL_SHAPE_WIDTH];
}

int VglShape::getHeight()
{
  if (this->ndim == 1)
  {
    return 1;
  }
  return this->shape[VGL_SHAPE_HEIGHT];
}

int VglShape::getLength()
{
  return this->shape[VGL_SHAPE_LENGTH];
}

/* WidthIn refers to the width of the internal representation of a 1D array stored as
   2D image. Textures have a maximum width limited to a few thousand pixels, so, 
   to represent long 1D arrays, they are broken to fit in a 2D texture.

   The dimensions of the internal representation are stored inside the shape structure.

   The actual array has height = 1 and width = array length.
 */
int VglShape::getWidthIn()
{
  return this->shape[VGL_SHAPE_WIDTH];
}

int VglShape::getHeightIn()
{
  return this->shape[VGL_SHAPE_HEIGHT];
}

int VglShape::getNFrames()
{
  int nframes = 1;
  int ndim = this->getNdim();
  for (int i = 3; i <= ndim; i++)
  {
    nframes *= this->shape[i];
  }
  return nframes;
}


int VglShape::findBitsPerSample(int depth)
{
  return (depth & 255);
}

int VglShape::findWidthStep(int bps, int width, int nChannels)
{
  if (bps == 1){
    return (width - 1) / VGL_PACK_SIZE_BITS + 1;
  }
  if (bps < 8)
  {
    printf("%s:%s: Error: bits per sample = %d < 8 and != 1. Image depth may be wrong.\n", __FILE__, __FUNCTION__, bps);
    exit(1);
  }
  return (bps / 8) * nChannels * width;
}



#ifdef __OPENCL__
/** Return shape as VglClShape

    Return shape as VglClShape, type suitable for passing shape as parameter
    to OpenCl kernel.

 */
VglClShape* VglShape::asVglClShape()
{
  VglClShape* result = new VglClShape;

  result->ndim = this->ndim;
  result->size = this->size;
  for (int i = 0; i <= VGL_MAX_DIM; i++)
  {
    result->shape[i] = this->shape[i];
    result->offset[i] = this->offset[i];
  }
  if (this->ndim == 1)
  {
    result->shape[VGL_SHAPE_WIDTH]  = this->getWidth();
    result->offset[VGL_SHAPE_WIDTH]  = result->shape[VGL_SHAPE_WIDTH-1] * result->offset[VGL_SHAPE_WIDTH-1];
    result->shape[VGL_SHAPE_HEIGHT] = this->getHeight();
    result->offset[VGL_SHAPE_HEIGHT]  = result->shape[VGL_SHAPE_HEIGHT-1] * result->offset[VGL_SHAPE_HEIGHT-1];
  }

#ifdef __DEBUG__
  printf("Inside VglShape::asVglClShape()===============\n");
  printf("ndim    = %d\n", result->ndim);
  printf("shape   = ");
  this->printArray(result->shape, result->ndim);
  printf("\n");
  printf("offset  = ");
  this->printArray(result->offset, result->ndim);
  printf("\n");
  printf("size    = %d\n", result->size);
  printf("bps     = %d\n", result->bps);
#endif

  return result;
}
#endif



