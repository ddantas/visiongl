/** N-dimensional dilation

    SHAPE directive passes an array sized 2*VGL_MAX_DIM. Parameter does not appear in wrapper parameter list
    SCALAR directive passes an scalar given by the expression that follows it. Parameter does not appear in wrapper parameter list.
    ARRAY directive passes an array sized as the expression between brackets.
    
  */

//SHAPE shape img_input->shape
//ARRAY convolution_window [window_size_x*window_size_y*window_size_z]
//SCALAR ndim img_input->ndim
//SCALAR VGL_MAX_DIM VGL_MAX_DIM

#include "vglClShape.h"
#include "vglClStrEl.h"

__kernel void vglClNdDilate(__global unsigned char* img_input, 
                            __global unsigned char* img_output,  
                            __constant VglClShape* img_shape,
                            __constant VglClStrEl* window)
{
  int x = get_global_id(0);
  int y = get_global_id(1);
  int w = get_global_size(0);
  int coord = y*w+x;

  int ires;
  int idim;
  ires = coord;
  unsigned char pmax = 0;
  int img_coord[VGL_ARR_SHAPE_SIZE];
  int win_coord[VGL_ARR_SHAPE_SIZE];

  for(int d = img_shape->ndim; d >= 1; d--)
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
      for(int d = window->ndim; d >= 1; d--)
      {
        int off = window->offset[d];
        idim = ires / off;
        ires = ires - idim * off;
        win_coord[d] = idim + img_coord[d];
        win_coord[d] = max(win_coord[d], 0);
        win_coord[d] = min(win_coord[d], img_shape->shape[d]);

        conv_coord += img_shape->offset[d] * win_coord[d];
      }
      pmax = max(pmax, img_input[conv_coord]);
    }
  }
  img_output[coord] = pmax;
}
