/** Negation of binary image img_input. Result is stored in img_output.

  */

#include "vglConst.h"

__kernel void vglClBinSwap(__read_only image2d_t img_input,
			   __write_only image2d_t img_output)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    uint4 p = read_imageui(img_input, smp, (int2)(coords.x, coords.y));
    VGL_PACK_CL_SHADER_TYPE result = 0;
    VGL_PACK_CL_SHADER_TYPE mask   = 1;
    VGL_PACK_CL_SHADER_TYPE input  = p.x;   
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
    write_imageui(img_output, coords, result);
}
