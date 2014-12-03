/*
*Implementation made by Helton Danilo at Universidade Federal de Sergipe
*The implementation follow the description of the book OpenCL Programming
*Guide, authors: Aaftab Munshi, Benedict R. Gaster, Timothy G. Mattson,
*James Fung, Dan Ginsburg published by Addison-Wesley, Page 395.
*/
//img_input: Image which the histogram is taken from
//hist: output histogram array
//this function takes histogram of partial sections of the img_input
//hist is not the final histogram yet
__kernel void vglCl3dPartialHistogram(__read_only image3d_t img_input, __global uint* hist, int nchannels)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
	
	int posx = get_global_id(0); //image width
	int height = get_image_height(img_input);
	int length = get_image_depth(img_input);
	
	//clear buffer
	for(uint y = 0; y < 256; y++)
	{
		for(uint z = 0; z < nchannels; z++)
		{
			hist[(posx*256*nchannels) + (y*nchannels) + z] = 0;
		}
	}
	
	//calculate hist for given x, variating y and z
    for(int y = 0; y < height; y++)
	{
		for(int z = 0; z < length; z++)
		{
			float4 clr = read_imagef(img_input, smp, (float4)(posx, y, z, 0));
			float* pclr = &clr; //quick fix to access clr as array
			for(uint ch = 0; ch < nchannels; ch++)
			{
				uchar c = convert_uchar_sat(pclr[ch] * 255.0f);
				atomic_inc(&hist[(posx*256*nchannels) + (c*nchannels) + ch]);
			}
		}
	}
}

//img_input: Image which the histogram is taken from
//hist: output histogram array
//this function takes histogram of partial sections of the img_input
//hist is not the final histogram yet
__kernel void vglCl2dPartialHistogram(__read_only image2d_t img_input, __global uint* hist, int nchannels)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
	
	uint posx = get_global_id(0);
	uint height = get_image_height(img_input);
	
	//clear buffer
	for(uint y = 0; y < 256; y++)
	{
		for(uint z = 0; z < nchannels; z++)
		{
			hist[posx*256*nchannels + y*nchannels + z] = 0;
		}
	}
	
	//calculate hist for given x, variating y
    for(uint y = 0; y < height; y++)
	{
		float4 clr = read_imagef(img_input, smp, (float2)(posx,y));
		float* pclr = &clr; //quick fix to access clr as array
		for(uint ch = 0; ch < nchannels; ch++)
		{
			uchar c = convert_uchar_sat(pclr[ch] * 255.0f);
			atomic_inc(&hist[(posx*256*nchannels) + (c*nchannels) + ch]);
		}
	}
}
//partial_hist: a matrix containing many partial_histogram
//hist: the output hist
//size: image width size
//calculates the sum on each component of partial_hist and the result puts into hist
__kernel void vglClSumPartialHistogram(__global uint* partial_hist, __global uint* hist, uint size, int nchannels)
{
	//goes from 0 to 256
	int posx = get_global_id(0);
	int posy = get_global_id(1);

	//init values
    if (posy == 0)
    {
	  for(int i = 0; i < nchannels; i++)
        hist[(posx*nchannels)+i]=0;
    }
	
	//for(int x = 0; x < size; x++)
	//{
    //Como é que essa porra tá funcionando?
    int x = posy;
	for(int i = 0; i < nchannels; i++)
	{
		atomic_add(&hist[(posx*nchannels) + i],partial_hist[(x*256*nchannels) + (posx*nchannels) + i]);
	}
	//}
}