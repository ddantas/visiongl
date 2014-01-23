/** Negative of src image. Result is stored in dst image.

  */

__kernel void vglClInvert(__read_only  image2d_t img_input,__write_only  image2d_t img_output)
{
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
	float4 p = read_imagef(img_input, smp, coords);
	p.xyz = 1.0 - p.xyz;
	write_imagef(img_output,coords,p);
}
