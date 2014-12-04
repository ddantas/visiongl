/*
*Implementation made by Helton Danilo at Universidade Federal de Sergipe
*/

//img_input, img_output: the input image and output image
//cdf_histogram: Cumulative sum of the histogram
//min: the min value of the cdf_histogram
__kernel void vglCl3dHistogramEq(__read_only image3d_t img_input,__write_only  image3d_t img_output,constant int* cdf_histogram, int min, int nchannels)
{
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
							  
	int image_size = get_image_width(img_input)*get_image_height(img_input)*get_image_depth(img_input);
	int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float4 pixel = read_imagef(img_input, smp, coords);
	float4 color = 1.0f;
	for(int i = 0; i < nchannels; i++)
	{
		uchar aux;
		if (i == 0) aux = convert_uchar_sat(pixel.x * 255.0f);
		if (i == 1) aux = convert_uchar_sat(pixel.y * 255.0f);
		if (i == 2) aux = convert_uchar_sat(pixel.z * 255.0f);
		if (i == 3) aux = convert_uchar_sat(pixel.w * 255.0f);
		float c = ((float)cdf_histogram[(aux*nchannels)+i]-min)/((float)image_size-min);
		if (i == 0) color.x = c;
		if (i == 1) color.y = c;
		if (i == 2) color.z = c;
		if (i == 3) color.w = c;
	}
	write_imagef(img_output,coords,color);
}

//img_input, img_output: the input image and output image
//transform: array containing the transformatin data, the sum(transform) must be same as width*height
__kernel void vglClGrayLevelTransform(__read_only image2d_t img_input,__write_only image2d_t img_output,constant int* transform)
{
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
							  
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	float4 pixel = read_imagef(img_input, smp, coords);
	float4 color = 0.0f;
	uchar aux = convert_uchar_sat(pixel.x * 255.0f);
	float c = transform[aux]/255.0f;
    color.x = c;
	write_imagef(img_output,coords,color);
}

//img_input, img_output: the input image and output image
//transform: array containing the transformatin data, the data in this array must be in range [0..255] to be mapped to [0..1]
__kernel void vglCl3dGrayLevelTransform(__read_only image3d_t img_input,__write_only image3d_t img_output,constant int* transform)
{
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
							  
	int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    float4 pixel = read_imagef(img_input, smp, coords);
	float4 color = 0.0f;
    uchar aux = convert_uchar_sat(pixel.x * 255.0f);
	color.x = transform[aux]/255.0f;
	write_imagef(img_output,coords,color);
}