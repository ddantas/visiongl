/** Minimum or intersection between two images.

    Minimum or intersection between img_input1 and img_input2. Result saved in img_output.
  */
__kernel void vglCl3dBinMin(__read_only image3d_t img_input1,
                          __read_only image3d_t img_input2,
			  __write_only image3d_t img_output)
{
    int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate


    uint4 p1 = read_imageui(img_input1, smp, (int4)(coords.x, coords.y, coords.z, 0));
    uint4 p2 = read_imageui(img_input2, smp, (int4)(coords.x, coords.y, coords.z, 0));
    uint4 result = p1 & p2;
    write_imageui(img_output, coords, result);
}
