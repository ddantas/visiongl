/** Erosion of src image by mask. Result is stored in dst image.

  */

// ARRAY convolution_window [window_size_x*window_size_y]
// SCALAR window_size_x
// SCALAR window_size_y

__kernel void vglClErosion(__read_only image2d_t img_input,
                          __write_only image2d_t img_output,
                          __constant float* convolution_window, 
                          __constant int* window_size_x, 
                          __constant int* window_size_y)
{
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                              CLK_FILTER_NEAREST;           //Don't interpolate
	
	int factorx = floor((float)*window_size_x / 2.0f);
	int factory = floor((float)*window_size_y / 2.0f);
	float4 pmin = (1.0,1.0,1.0,1.0);
	for(int i = 0; i < *window_size_x * *window_size_y; i++)
	{
		int xi = (i / *window_size_x) - factorx;
		int yi = (i % *window_size_y) - factory;
		float4 p = read_imagef(img_input, smp, (int2)(coords.x + xi,coords.y + yi));
		if (convolution_window[i] != 0)
			pmin = min(p,pmin);
	}
	write_imagef(img_output,coords,pmin);
}
