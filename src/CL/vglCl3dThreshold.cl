/** Threshold of src image by float parameter. Result is stored in dst image.

  */
__kernel void vglCl3dThreshold(__read_only image2d_t src,
                        __write_only image2d_t dst,
                        float thresh)
{
	int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
	float4 p = read_imagef(src, smp, coords);
	
	if( p.x < thresh)
		p.x = 0.0f;
	else
		p.x = 1.0f;

	if( p.y < thresh)
		p.y = 0.0f;
	else
		p.y = 1.0f;

	if( p.z < thresh)
		p.z = 0.0f;
	else
		p.z = 1.0f;

	write_imagef(dst, coords, p);
}
