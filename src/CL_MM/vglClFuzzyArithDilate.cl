/** Erosion of src image by mask. Result is stored in dst image.

  */

// ARRAY convolution_window [window_size_x*window_size_y]
// SCALAR window_size_x
// SCALAR window_size_y

__kernel void vglClFuzzyArithDilate(__read_only image2d_t img_input,
                                __write_only image2d_t img_output,
                                __constant float* convolution_window, 
                                int window_size_x, 
                                int window_size_y)
{
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                              CLK_FILTER_NEAREST;           //Don't interpolate
	
	int factorx = floor((float)window_size_x / 2.0f);
	int factory = floor((float)window_size_y / 2.0f);
	int conv_controller = 0;
	float4 pmax = (0,0,0,0);
  for(int j = -factory; j <= factory; j++)
	{
		for(int i = -factorx; i <= factorx; i++)
		{
			float4 a = read_imagef(img_input, smp, (int2)(coords.x + i,coords.y + j));
			float b = convolution_window[conv_controller];
			float4 S = sqrt(min(a,b)*((a+b)/2));
			pmax = max(pmax,S);
			conv_controller++;
		}
	}
	write_imagef(img_output,coords,pmax);
}