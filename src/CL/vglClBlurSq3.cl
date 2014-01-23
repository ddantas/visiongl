/** Convolution of src image by mask. Result is stored in dst image.

  */

__kernel void vglClBlurSq3(__read_only image2d_t img_input,__write_only image2d_t img_output)
{
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                              CLK_FILTER_NEAREST;           //Don't interpolate

	float4 result = (0,0,0,0);
	float convolution_window[9] = {1.0/16.0, 0.0/16.0, 1.0/16.0, 
                                       2.0/16.0, 0.0/16.0, 2.0/16.0, 
                                       1.0/16.0, 0.0/16.0, 1.0/16.0, };
	
	for(int i = 0; i < 9; i++)
	{
		int xi = (i / 3) - 1;
		int yi = (i % 3) - 1;
		float4 p = read_imagef(img_input, smp, (int2)(coords.x + xi,coords.y + yi));
		p.xyz *= convolution_window[i];
		result += p;
	}
	write_imagef(img_output, coords, result);
}
