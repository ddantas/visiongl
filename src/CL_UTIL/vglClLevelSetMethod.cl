/*
*Implementation made by Helton Danilo at Universidade Federal de Sergipe
*Adapted from Erik Smistad, implementation can be found here: 
*https://github.com/smistad/OpenCL-Level-Set-Segmentation/blob/master/kernels.cl
*/
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