/** Threshold of src image by float parameter. Result is stored in dst image.

  */
__kernel void vglCl3dMergeZByMean(__read_only image3d_t src,
                                  __write_only image3d_t dst,
								  int number_of_merges)
{
	int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
							  
	float4 pmean = (0,0,0,0);						  
	for (int i = 0; i < number_of_merges; i++)
	{
		int4 lcoords = (int4)(get_global_id(0), get_global_id(1), (get_global_id(2)*number_of_merges)+i, 0);
		float4 p1 = read_imagef(src, smp, lcoords);
		pmean += p1;
	}
	
	pmean /= number_of_merges;
	
	write_imagef(dst, coords, pmean);
}
