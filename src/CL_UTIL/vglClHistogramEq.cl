/*
*Implementation made by Helton Danilo at Universidade Federal de Sergipe
*The implementation follow the description of the algorithm explained by 
*Mark Harris from NVIDIA, obtained through the following link:
*http://www.eecs.umich.edu/courses/eecs570/hw/parprefix.pdf
*page 7-11. Some otimization was made to keep the last element
*/

//CumSum: Array containing the data to be Cumulative Sum
//size: length of the cumsum
__kernel void vglClCumSum(__global int* cumsum, int size)
{
	int index = get_global_id(0);
	int global_size = get_global_size(0)*2;
	int offset = 1;
	for(int d = global_size>>1; d> 0; d>>=1)
	{
		if (index < d)
		{
			int ai = offset*(2*index+1)-1;
			int bi = offset*(2*index+2)-1;
			
			cumsum[bi] += cumsum[ai];
		}
		offset *= 2;
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
	
	//saves last elem for putting it again at end
	int last_elem = cumsum[global_size-1];
	
	//set last elem 0 so downsweep be possible
	if(index == 0)
		cumsum[global_size-1] = 0;
	
	//downsweep
	int d;
	for(d = 1; d < global_size/2; d*=2)
	{
		offset = offset >> 1;
		barrier(CLK_GLOBAL_MEM_FENCE);
		if (index < d)
		{
			int ai = (offset*((2*index)+1))-1;
			int bi = (offset*((2*index)+2))-1;
			
			int aux = cumsum[ai];
			cumsum[ai] = cumsum[bi];
			cumsum[bi] += aux;
		}
		
	}
	//last step of down sweep, this resemble the last elem into the array
	d *= 2;
	offset = offset >> 1;
	barrier(CLK_GLOBAL_MEM_FENCE);
	if (index == 0)
	{
		//do nothing
	}
	else if (index < d)
	{
		int ai = (offset*((2*index)+1))-1;
		int bi = (offset*((2*index)+2))-1;
		int aux1 = cumsum[ai];
		int aux2 = cumsum[bi];
		
		barrier(CLK_GLOBAL_MEM_FENCE);
		
		cumsum[ai-1] = aux2;
		cumsum[bi-1] = aux2+aux1;
	}
	cumsum[global_size-1] = last_elem;
}

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
							  
	int image_size = get_image_width(img_input)*get_image_height(img_input);
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	float4 pixel = read_imagef(img_input, smp, coords);
	float4 color = 0.0f;
	uchar aux = convert_uchar_sat(pixel.x * 255.0f);
	float c = transform[aux]/((float)image_size);
    color.x = c;
	write_imagef(img_output,coords,color);
}

//img_input, img_output: the input image and output image
//transform: array containing the transformatin data, the sum(transform) must be same as width*height*length
__kernel void vglCl3dGrayLevelTransform(__read_only image3d_t img_input,__write_only image3d_t img_output,constant int* transform)
{
	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                              CLK_ADDRESS_CLAMP |           //Clamp to zeros
                              CLK_FILTER_NEAREST;           //Don't interpolate
							  
	int image_size = get_image_width(img_input)*get_image_height(img_input)*get_image_depth(img_input);
	int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float4 pixel = read_imagef(img_input, smp, coords);
	float4 color = 0.0f;
	uchar aux = convert_uchar_sat(pixel.x * 255.0f);
	float c = transform[aux]/((float)image_size);
    color.x = c;
	write_imagef(img_output,coords,color);
}