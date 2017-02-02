/** Generate ROI.

    Generate ROI (Region Of Interest). Useful to be used as mask to do intersection
    with other images.

  */

#include "vglConst.h"

__kernel void vglCl3dBinRoi(__write_only image3d_t img_output,
			    int x0, int y0, int z0, int xf, int yf, int zf)
{
    int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate

if (coords.y == 0 && coords.z == 0)
  printf("%d\n", coords.x);  
   
    uint4 result = 0;
    for (int bit = 0; bit < VGL_PACK_SIZE_BITS; bit++)
    {
      int i_img = coords.y;
      int j_img = VGL_PACK_SIZE_BITS * coords.x + bit;
      int k_img = coords.z;
      unsigned int result_bit = 0;
 
      if ( (i_img >= y0) && (i_img <= yf) && 
           (j_img >= x0) && (j_img <= xf) && 
           (k_img >= z0) && (k_img <= zf) )
      {
        result_bit = 1;
      }

      result += result_bit << bit;
    }
    write_imageui(img_output, coords, result);
}
