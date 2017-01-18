/** Convert binary image to grayscale.

    Convert binary image to grayscale.
  */
__kernel void vglClBinToGray(__read_only image2d_t img_input,
                             __write_only image2d_t img_output)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    float4 result = 0.0;
    uint4 p = read_imageui(img_input, smp, (int2)(coords.x, coords.y));
    for (int bit = 0; bit < 8; bit++)
    {
      uint4 result_bit;
      result_bit.x = p.x & (1 << bit);
      if (result_bit.x)
        result.x = 0.0;
      else
        result.x = 1.0;
      write_imagef(img_output, (int2)(8*coords.x + 7 - bit, coords.y), result);
    }
}
