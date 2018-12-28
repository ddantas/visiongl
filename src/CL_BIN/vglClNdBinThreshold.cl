/** Threshold of img_input by parameter. if the pixel is below thresh,
    the output is 0, else, the output is 1. Result is stored in img_output.
    Input image is 8bpp and output is 1bpp.
  */

// SHAPE in_shape  (img_input->vglShape->asVglClShape())
// SHAPE out_shape (img_output->vglShape->asVglClShape())

#include "vglClShape.h"
#include "vglConst.h"

__kernel void vglClNdBinThreshold(__global char* img_input,
                                  __global VGL_PACK_CL_SHADER_TYPE* img_output,
                                  unsigned char thresh,
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

  //VGL_PACK_OUTPUT_SWAP_MASK

  VGL_PACK_CL_SHADER_TYPE result = 0;
  for (int bit = 0; bit < VGL_PACK_SIZE_BITS; bit++)
  {
    unsigned char p = img_input[coord  * VGL_PACK_SIZE_BITS + bit];
    if (p >= thresh)
    {
      result = result | ( (VGL_PACK_CL_SHADER_TYPE) 1 << (VGL_PACK_CL_SHADER_TYPE) bit);
      //result = result | ( (VGL_PACK_CL_SHADER_TYPE) outputSwapMask[bit]);
    }
  }
  img_output[coord] = result;
}
