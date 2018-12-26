/** Convert binary image to grayscale.

    Convert binary image to grayscale.
  */

// SHAPE in_shape  (img_input->vglShape->asVglClShape())
// SHAPE out_shape (img_output->vglShape->asVglClShape())

#include "vglConst.h"
#include "vglClShape.h"

__kernel void vglCl3dBinToGray(__read_only image3d_t img_input,
                             __write_only image3d_t img_output,
                             __constant VglClShape* in_shape,
                             __constant VglClShape* out_shape)
{
  int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    float4 result = 0.0;
    uint4 p = read_imageui(img_input, smp, (int4)(coords.x, coords.y, coords.z, 0));
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

      uint4 result_bit;
      result_bit.x = p.x & (1 << bit);
      if (result_bit.x)
        result.x = 1.0;
      else
        result.x = 0.0;

      if (VGL_PACK_SIZE_BITS * coords.x + off < out_shape->shape[1])
        write_imagef(img_output, (int4)(VGL_PACK_SIZE_BITS * coords.x + off, coords.y, coords.z, 0), result);

    }
}
