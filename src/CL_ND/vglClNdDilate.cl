/** N-dimensional dilation

    SHAPE directive passes an array sized 2*VGL_MAX_DIM. Parameter does not appear in wrapper parameter list
    SCALAR directive passes an scalar given by the expression that follows it. Parameter does not appear in wrapper parameter list.
    ARRAY directive passes an array sized as the expression between brackets.
    

typedef 
struct 
VglClShape{ 
  cl_int ndim;
  cl_int shape[11];
  cl_int offset[11];
  cl_int size;
} VglClShape;



  */

//SHAPE shape img_input->shape
//ARRAY convolution_window [window_size_x*window_size_y*window_size_z]
//SCALAR ndim img_input->ndim
//SCALAR VGL_MAX_DIM VGL_MAX_DIM

#include "vglClShape.h"
#include "vglClStrEl.h"

__kernel void vglClNdDilate(__global char* img_input, 
                            __global char* img_output,  
                            __constant VglClShape* img_shape,
                            __constant VglClStrEl* window)
{
  int x = get_global_id(0);
  int y = get_global_id(1);
  int w = get_global_size(0);
  int coord = y*w+x;

  char pmax = 0;

  int ires = coord;
  int idim;
  int img_coord[VGL_MAX_DIM+1];
  int win_coord[VGL_MAX_DIM+1];
  for(int d = 1; d <= window->ndim; d++)
  {
    int off = img_shape->offset[d];
    idim = ires / off;
    ires = ires - idim * off;
    img_coord[d] = idim - ((window->shape[d] - 1) / 2);
  }

  int conv_coord = 0;
  for(int i = 0; i < window->size; i++)
  {
    if (!(window->data[i] == 0))
    {
      ires = i;
      conv_coord = 0;
      for(int d = 1; d <= window->ndim; d++)
      {
        int off = window->offset[d];
        idim = ires / off;
        ires = ires - idim * off;
        win_coord[d] = idim + img_coord[d];
        win_coord[d] = max(win_coord[d], 0);
        win_coord[d] = min(win_coord[d], img_shape->shape[d]);

        conv_coord += win_coord[d] * img_shape->offset[d];
      }
      pmax = max(pmax, img_input[conv_coord]);
      pmax = conv_coord;
    }
  }

  img_output[coord] = img_input[img_shape->size - coord];
}


/*
__kernel void vglClNdNot(__read_only image3d_t img_input, __write_only image3d_t img_output)
{
  int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
  write_imagef(img_output, coords, get_global_id(2) /17.0);
}
*/
