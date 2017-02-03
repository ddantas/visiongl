/** Negation of binary image img_input. Result is stored in img_output.

  */

#include "vglConst.h"

__kernel void vglClNdBinSwap(__global VGL_PACK_CL_SHADER_TYPE*  img_input,
		             __global VGL_PACK_CL_SHADER_TYPE*  img_output)
{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

    VGL_PACK_CL_SHADER_TYPE result = 0;
    VGL_PACK_CL_SHADER_TYPE mask   = 1;
    VGL_PACK_CL_SHADER_TYPE input  = img_input[coord];
    VGL_PACK_OUTPUT_SWAP_MASK

    for (int i = 0; i < VGL_PACK_SIZE_BITS; i++)
    {
      //printf("out[%2d] = %8x mask = %8x\n", i, output[i], mask);
      if (input & mask)
      {
        result = result | outputSwapMask[i];
      }
      mask = mask << 1;
    }
    img_output[coord] = result;
}
