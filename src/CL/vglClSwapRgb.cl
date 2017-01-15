/** Swap R and B channels.
  */
__kernel void vglClSwapRgb(__read_only image2d_t src, __write_only image2d_t dst)
{
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
	float4 p = read_imagef(src, smp, coords);
        float  f;
	
	f = p.x;
	p.x = p.z;
        p.z = f;

	write_imagef(dst, coords, p);
}
