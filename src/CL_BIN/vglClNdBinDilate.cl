/** N-dimensional dilation

    SHAPE directive passes a structure with size of each dimension, offsets and number of dimensions. Parameter does not appear in wrapper parameter list. The C expression between parenthesis returns the desired shape of type VglClShape.
    
  */

//SHAPE img_shape (img_input->vglShape->asVglClShape())

#include "vglClShape.h"
#include "vglClStrEl.h"

__kernel void vglClNdBinDilate(__global VGL_PACK_CL_SHADER_TYPE* img_input, 
                            __global VGL_PACK_CL_SHADER_TYPE* img_output,  
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
  VGL_PACK_CL_SHADER_TYPE result = 0;
  int img_coord[VGL_ARR_SHAPE_SIZE];
  int win_coord[VGL_ARR_SHAPE_SIZE];

  for(int d = img_shape->ndim; d >= 1; d--)
  {
    int off = img_shape->offset[d];
    idim = ires / off;
    ires = ires - idim * off;
    if (d == VGL_SHAPE_WIDTH)
      img_coord[d] = VGL_PACK_SIZE_BITS * idim + ((window->shape[d] - 1) / 2); //In erosion, replace + with -
    else
      img_coord[d] =                      idim + ((window->shape[d] - 1) / 2); //In erosion, replace + with -
  }

  for (int bit = 0; bit < VGL_PACK_SIZE_BITS; bit++)
  {
    VGL_PACK_CL_SHADER_TYPE pmax = 0;
    for(int i = 0; i < window->size && pmax == 0; i++)
    {
      int j_bit;
      int j_byte;
      int conv_coord;
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
          win_coord[d] = - idim + img_coord[d]; //In erosion, remove the -

          if (d == VGL_SHAPE_WIDTH)
	  {
            win_coord[d] += VGL_PACK_SIZE_BITS - 1 - bit;
            win_coord[d] = clamp(win_coord[d], 0, img_shape->shape[d]-1);
            j_bit = img_shape->offset[d] * win_coord[d];
            j_byte = j_bit / VGL_PACK_SIZE_BITS;
            j_bit  = j_bit - j_byte * VGL_PACK_SIZE_BITS;
            conv_coord += j_byte;
	  }
          else
	  {
            win_coord[d] = clamp(win_coord[d], 0, img_shape->shape[d]-1);
            conv_coord += img_shape->offset[d] * win_coord[d];
	  }
        }
        VGL_PACK_CL_SHADER_TYPE p = img_input[conv_coord] & (1 << j_bit);
        VGL_PACK_CL_SHADER_TYPE result_bit;
        if (p)
          result_bit = 1;
        else
          result_bit = 0;
        pmax = max(pmax, result_bit);
      }
    }
    result += pmax << (VGL_PACK_SIZE_BITS - 1 - bit);
  }
  img_output[coord] = result;
}
