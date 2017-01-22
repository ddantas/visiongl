/** Generate ROI.

    Generate ROI (Region Of Interest). Useful to be used as mask to do intersection
    with other images.

  */

__kernel void vglClBinRoi(__write_only image2d_t img_output,
                          int x0, int y0, int xf, int yf)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
    
    uint4 result = 0;
    for (int bit = 0; bit < 8; bit++)
    {
      int i_img = coords.y;
      int j_img = 8 * coords.x + 7 - bit;
      unsigned int result_bit = 0;
 
      if ( (i_img >= y0) && (i_img <= yf) && (j_img >= x0) && (j_img <= xf) )
      {
        result_bit = 1;
      }

      result += result_bit << bit;
    }
    write_imageui(img_output, coords, result);
}
