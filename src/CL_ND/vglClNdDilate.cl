/** N-dimensional dilation

    SHAPE directive passes a structure with size of each dimension, offsets and number of dimensions. Parameter does not appear in wrapper parameter list. The C expression between parenthesis returns the desired shape of type VglClShape.
    
  */

//SHAPE img_shape (img_input->vglShape->asVglClShape())

#include "vglClShape.h"
#include "vglClStrEl.h"

__kernel void vglClNdDilate(__global unsigned char* img_input, 
                            __global unsigned char* img_output,  
                            __constant VglClShape* img_shape,
                            __constant VglClStrEl* window)
{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif


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
  //barrier(CLK_LOCAL_MEM_FENCE);
  for(int i = 0; i < window->size; i++)
  {
    if (!(window->data[i] == 0))
    {
      ires = i;
      conv_coord = 0;
      for(int d = img_shape->ndim; d > window->ndim; d--)
      {
        conv_coord += img_shape->offset[d] * img_coord[d];
      }
      for(int d = window->ndim; d >= 1; d--)
      {
        int off = window->offset[d];
        idim = ires / off;
        ires = ires - idim * off;
        win_coord[d] = idim + img_coord[d];
        win_coord[d] = clamp(win_coord[d], 0, img_shape->shape[d]-1);

        conv_coord += img_shape->offset[d] * win_coord[d];
      }
      pmax = max(pmax, img_input[conv_coord]);
    }
    //barrier(CLK_LOCAL_MEM_FENCE);
  }
  img_output[coord] = pmax;
}
