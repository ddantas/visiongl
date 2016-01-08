
#include <vglStrEl.h>
#include <vglShape.h>

void VglStrEl::VglCreateStrEl(float* data, VglShape* vglShape)
{
  int size = vglShape->getSize();
  this->vglShape = new VglShape(vglShape);
  this->data = (float*) malloc(sizeof(float) * size);

  for (int i = 0; i < size; i++)
  {
    this->data[i] = data[i];
  }
}

VglStrEl::VglStrEl(float* data, VglShape* vglShape)
{
  this->VglCreateStrEl(data, vglShape);
}

VglStrEl::VglStrEl(int type, int ndim)
{
  int shape[VGL_MAX_DIM];
  shape[0] = 1;
  for (int i = 1; i <= ndim; i++)
  {
    shape[i] = 3;
  }
  VglShape* vglShape = new VglShape(shape, ndim);

  int size = vglShape->getSize();
  float* data = (float*) malloc(sizeof(float) * size);
  int index;
  switch(type)
  {
    case(VGL_STREL_CROSS):
    {
      int coord[VGL_MAX_DIM+1];
      coord[0] = 0;
      for (int i = 0; i < size; i++)
      {
        data[i] = 0.0;
      }

      for (int d = 1; d <= ndim; d++)
      {
        coord[d] = 1;
      }
      index = vglShape->getIndexFromCoord(coord);
      data[index] = 1.0;

      for (int d = 1; d <= ndim; d++)
      {
        coord[d] = 0;
        index = vglShape->getIndexFromCoord(coord);
        data[index] = 1.0;

        coord[d] = 2;
        index = vglShape->getIndexFromCoord(coord);
        data[index] = 1.0;

        coord[d] = 1;
      }
    }
    break;
    case(VGL_STREL_GAUSS):
    {
      printf("STREL GAUSS entrou\n");
      int coord[VGL_MAX_DIM+1];
      coord[0] = 0;
      int size = vglShape->getSize();
      printf("STREL GAUSS size = %d\n", size);
      for (int i = 0; i < size; i++)
      {
        printf("STREL GAUSS i = %d\n", i);
        float val = 1.0;
        vglShape->getCoordFromIndex(i, coord);
        for (int d = 1; d <= ndim; d++)
	{
          if (coord[d] == 1)
	  {
            val *= .5;
	  }
          else
	  {
            val *= .25;
	  }        
	}
        printf("STREL GAUSS i = %d, val = %f\n", i, val);
        data[i] = val;
      }
    }
    break;
    case(VGL_STREL_CUBE):
    default:
    {
      for (int i = 0; i < size; i++)
      {
        data[i] = 1.0;
      }
    }
  }
  this->VglCreateStrEl(data, vglShape);
  delete vglShape;
  free(data);
}

VglStrEl::~VglStrEl()
{
  free (this->data);
  delete &this->vglShape;
}

/** Print information about structuring element.

    Print dimensions, offsets, total size and number of dimensions.

 */
void VglStrEl::print(char* msg)
{
  if (msg){
    printf("====== %s:\n", msg);
  }
  else
  {
    printf("====== VglStrEl->print():\n");
  }
  this->vglShape->printInfo();
  int size = this->getSize();
  int ndim = this->getNdim();
  int* offset = this->getOffset();
  float* data = this->getData();
  int idim, ires;
  for(int i = 0; i < size; i++)
  {
    if (i % 3 == 0)
    {
      if (i > 0)
      {
        printf("\n");
      }
      ires = i;
      printf("i = %3d {", i);
      for(int d = ndim; d >= 2; d--)
      {
        idim = ires / offset[d];
        ires = ires - idim * offset[d];
        printf("%2d ", idim);
      }
      printf("}  ");
    }

    printf(" %1.4f ", data[i]);

  }
  printf("\n");
}

/*
void VglStrEl::VglReleaseStrEl(vglStrEl** p_strel){
  free (this->data)
  delete(*p_strel);
}
*/

/** /brief Get size.

    Get total size of shape. Is the product of all shape array positions, including
    number of channels. Total size of shape in bytes is getSize*sizeof(type).
 */
float* VglStrEl::getData()
{
  return this->data;
}

/** /brief Get size.

    Get total size of shape. Is the product of all shape array positions, including
    number of channels. Total size of shape in bytes is getSize*sizeof(type).
 */
int VglStrEl::getSize()
{
  return this->vglShape->getSize();
}

/** /brief Get number of pixels.

    Get number of pixels of shape. Is the product of all shape array positions, 
    not including number of channels.
 */
int VglStrEl::getNpixels()
{
  return this->vglShape->getNpixels();
}

/** /brief Get number of dimensions.

    Get number of dimension of shape. Shape array valid positions is this->getNdim()+1
    as position 0 contains number of channels, which is not considered a dimension.
 */
int VglStrEl::getNdim()
{
  return this->vglShape->getNdim();
}

/** /brief Get shape array.

    Get shape array, with size of each dimension and number of channels in position 0.
 */
int* VglStrEl::getShape()
{
  return this->vglShape->getShape();
}

/** /brief Get offset array.

    Get offset array.
 */
int* VglStrEl::getOffset()
{
  return this->vglShape->getOffset();
}
