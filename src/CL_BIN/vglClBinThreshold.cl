/** Threshold of grayscale image with binary result.

    Threshold of grayscale image img_input. Result is binary, stored in img_output. Parameter
    thresh is float between 0.0 and 1.0.
  */

#include "vglConst.h"

__kernel void vglClBinThreshold(__read_only image2d_t img_input,
			  __write_only image2d_t img_output,
                          float thresh)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    uint4 result = 0;
    for (int bit = 0; bit < VGL_PACK_SIZE_BITS; bit++)
    {
      int off;
      if (VGL_PACK_SIZE_BYTES == 1)
      {
        off = 7 - bit;
      }
      else
      {
        int byte = bit / 8;
        int rem = bit - 8 * byte;
        off = byte * 8 + 7 - rem;
      }

      float4 p = read_imagef(  img_input, smp, (int2)( VGL_PACK_SIZE_BITS * coords.x + off, 
                                                       coords.y )  );
      uint4 result_bit;
      if (p.x >= thresh)
        result_bit.x = 1;
      else
        result_bit.x = 0;
      result += result_bit.x << bit;
    }
    write_imageui(img_output, coords, result);
}
