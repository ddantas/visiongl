/** Negation of binary image img_input. Result is stored in img_output.

  */

#include "vglConst.h"

__kernel void vglClBinNot(__read_only image2d_t img_input,
			  __write_only image2d_t img_output)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    uint4 p = read_imageui(img_input, smp, (int2)(coords.x, coords.y));
    write_imageui(img_output, coords, VGL_PACK_MAX_UINT & ~p);
}
