/** Copy of binary image img_input to img_output.

  */
__kernel void vglCl3dBinCopy(__read_only image3d_t img_input,
                             __write_only image3d_t img_output)
{
  int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    uint4 result = 0;
    uint4 p = read_imageui(img_input, smp, (int4)(coords.x, coords.y, coords.z, 0));
    write_imageui(img_output, coords, p);
}
