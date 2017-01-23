/** Threshold of grayscale image with binary result.

    Threshold of grayscale image img_input. Result is binary, stored in img_output. Parameter
    thresh is float between 0.0 and 1.0.
  */
__kernel void vglCl3dBinThreshold(__read_only image3d_t img_input,
			  __write_only image3d_t img_output,
                          float thresh)
{
    int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate

    uint4 result = 0;
    for (int bit = 0; bit < 8; bit++)
    {
      float4 p = read_imagef(img_input, smp, (int4)(8*coords.x + 7 - bit, coords.y, coords.z, 0));
      uint4 result_bit;
      if (p.x >= thresh)
        result_bit.x = 1;
      else
        result_bit.x = 0;
      result += result_bit.x << bit;
    }
    write_imageui(img_output, coords, result);
}
