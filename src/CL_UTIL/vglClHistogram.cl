__kernel void vglCl3dPartialHistogram(__read_only image3d_t img_input, __global uint* hist)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
	
	int posx = get_global_id(0); //image width
	int height = get_image_height(img_input);
	int length = get_image_depth(img_input);
	
	//clear buffer
	/*for(uint y = 0; y < 256; y++)
	{
		for(uint z = 0; z < 3; z++)
		{
			hist[posx*256*3 + y*3 + z] = 0;
		}
	}*/
	
	//calculate hist for given x, variating y and z
    for(int y = 0; y < height; y++)
	{
		for(int z = 0; z < length; z++)
		{
			float4 clr = read_imagef(img_input, smp, (float4)(posx, y, z, 0));
			uchar indx_x, indx_y, indx_z;
			indx_x = convert_uchar_sat(clr.x * 255.0f);
			indx_y = convert_uchar_sat(clr.y * 255.0f);
			indx_z = convert_uchar_sat(clr.z * 255.0f);
			hist[(posx*256*3) + (indx_x*3) + 0]++;
			hist[(posx*256*3) + (indx_y*3) + 1]++;
			hist[(posx*256*3) + (indx_z*3) + 2]++;
		}
	}
}

__kernel void vglCl2dPartialHistogram(__read_only image2d_t img_input, __global uint* hist)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
	
	uint posx = get_global_id(0);
	uint height = get_image_height(img_input);
	
	//clear buffer
	for(uint y = 0; y < 256; y++)
	{
		for(uint z = 0; z < 3; z++)
		{
			hist[posx*256*3 + y*3 + z] = 0;
		}
	}
	
	//calculate hist for given x, variating y
    for(uint y = 0; y < height; y++)
	{
		float4 clr = read_imagef(img_input, smp, (float2)(posx,y));
		uchar indx_x, indx_y, indx_z;
		indx_x = convert_uchar_sat(clr.x * 255.0f);
		indx_y = convert_uchar_sat(clr.y * 255.0f);
		indx_z = convert_uchar_sat(clr.z * 255.0f);
		hist[(posx*256*3) + (indx_x*3) + 0]++;
		hist[(posx*256*3) + (indx_y*3) + 1]++;
		hist[(posx*256*3) + (indx_z*3) + 2]++;
	}
}

__kernel void vglClSumPartialHistogram(__global uint* partial_hist, __global uint* hist, uint size)
{
	//goes from 0 to 256
	int posx = get_global_id(0);
	
	//init values
	hist[(posx*3)+0]=0;
	hist[(posx*3)+1]=0;
	hist[(posx*3)+2]=0;
	for(int x = 0; x < 512; x++)
	{
		hist[(posx*3) + 0] += partial_hist[(x*256*3) + (posx*3) + 0];
		hist[(posx*3) + 1] += partial_hist[(x*256*3) + (posx*3) + 1];
		hist[(posx*3) + 2] += partial_hist[(x*256*3) + (posx*3) + 2];
	}
}