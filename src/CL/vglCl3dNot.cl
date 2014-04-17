/** Direct copy from src to dst.

  */

__kernel void vglCl3dNot(__read_only image3d_t img_input,__write_only image3d_t img_output)
{
	int3 coords = (int3)(get_global_id(0), get_global_id(1), get_global_id(2));
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
	float4 p = read_imagef(img_input, smp, coords);
	float4 a = (1.0f, 1.0f, 1.0f, 0f) - p;
	write_imagef(img_output,coords,p);
}
