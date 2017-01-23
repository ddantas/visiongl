/** Negation of binary image img_input. Result is stored in img_output.

  */
__kernel void vglClBinNot(__read_only image2d_t img_input,
			  __write_only image2d_t img_output)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    uint4 p = read_imageui(img_input, smp, (int2)(coords.x, coords.y));
    /*
    uint4 result = 0;
    for (int bit = 0; bit < 8; bit++)
    {
      uint4 result_bit;
      result_bit.x = p.x & (1 << bit);
      if (result_bit.x > 0)
        result_bit.x = 0;
      else
        result_bit.x = 1;
      result += result_bit.x << bit;
    }
    */
    //write_imageui(img_output, coords, 0x7f ^ p);
    write_imageui(img_output, coords, 0xff & ~p);
}
