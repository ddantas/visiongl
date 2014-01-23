/** Threshold of src image by float parameter. Result is stored in dst image.

  */
__kernel void threshold(__read_only image2d_t img_input,__write_only image2d_t img_output,__constant float* thresh)
{
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
	float4 p = read_imagef(img_input, smp, coords);
	
	if( p.x < *thresh)
		p.x = 0.0f;
	else
		p.x = 1.0f;

	if( p.y < *thresh)
		p.y = 0.0f;
	else
		p.y = 1.0f;

	if( p.z < *thresh)
		p.z = 0.0f;
	else
		p.z = 1.0f;

	write_imagef(img_output,coords,p);
}
