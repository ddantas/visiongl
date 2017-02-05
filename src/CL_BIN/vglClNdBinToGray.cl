/** Convert binary image to grayscale.

    Convert binary image to grayscale.
  */

// SHAPE in_shape  (img_input->vglShape->asVglClShape())
// SHAPE out_shape (img_output->vglShape->asVglClShape())

#include "vglClShape.h"

__kernel void vglClNdBinToGray(__global VGL_PACK_CL_SHADER_TYPE* img_input,
                               __global char* img_output,
                               __constant VglClShape* in_shape,
                               __constant VglClShape* out_shape)
{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

  VGL_PACK_OUTPUT_SWAP_MASK

  unsigned char result = 0;
  VGL_PACK_CL_SHADER_TYPE p = img_input[coord];
  for (int bit = 0; bit < VGL_PACK_SIZE_BITS; bit++)
  {
    VGL_PACK_CL_SHADER_TYPE result_bit = p & ( (VGL_PACK_CL_SHADER_TYPE) 1 << (VGL_PACK_CL_SHADER_TYPE) bit ); //outputSwapMask[bit];
    if (result_bit)
      img_output[coord * VGL_PACK_SIZE_BITS + bit] = 255;
    else
      img_output[coord * VGL_PACK_SIZE_BITS + bit] = 0;
  }
}
