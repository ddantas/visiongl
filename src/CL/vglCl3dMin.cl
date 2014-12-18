/** Direct copy from src to dst.

  */

__kernel void vglCl3dMin(__read_only image3d_t img_input1,__read_only image3d_t img_input2, __write_only image3d_t img_output)
{
	int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
	float4 p1 = read_imagef(img_input1, smp, coords);
	float4 p2 = read_imagef(img_input2, smp, coords);
    float4 minI;
    minI.x = min(p1.x,p2.x);
    minI.y = min(p1.y,p2.y);
    minI.z = min(p1.z,p2.z);
    minI.w = min(p1.w,p2.w);
	write_imagef(img_output, coords, minI);
}
