/** Negation of binary image img_input. Result is stored in img_output.

  */

#include "vglConst.h"

__kernel void vglCl3dBinSwap(__read_only image3d_t img_input,
			   __write_only image3d_t img_output)
{
    int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    uint4 p = read_imageui(img_input, smp, (int4)(coords.x, coords.y, coords.z, 0));
    VGL_PACK_CL_SHADER_TYPE result = 0;
    VGL_PACK_CL_SHADER_TYPE mask   = 1;
    VGL_PACK_CL_SHADER_TYPE input  = p.x;   
    VGL_PACK_CL_SHADER_TYPE output[32] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
                                          0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100, 
                                          0x800000, 0x400000, 0x200000, 0x100000, 0x080000, 0x040000, 0x020000, 0x010000, 
                                          0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000, 0x02000000, 0x01000000, };
    for (int i = 0; i < VGL_PACK_SIZE_BITS; i++)
    {
      //printf("out[%2d] = %8x mask = %8x\n", i, output[i], mask);
      if (input & mask)
      {
        result = result | output[i];
      }
      mask = mask << 1;
    }
    write_imageui(img_output, coords, result);
}
