/** Threshold of src image by float parameter. Result is stored in dst image.

  */
__kernel void vglCl3dMergeZByMax(__read_only image3d_t src,
                                  __write_only image3d_t dst,
								  int number_of_merges)
{
	int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
							  
	float4 pmax = (0,0,0,0);						  
	for (int i = 0; i < number_of_merges; i++)
	{
		int4 lcoords = (int4)(get_global_id(0), get_global_id(1), (get_global_id(2)*number_of_merges)+i, 0);
		float4 p = read_imagef(src, smp, lcoords);
		pmax.x = max(pmax.x, p.x);
	}
	
	write_imagef(dst, coords, pmax);
}
