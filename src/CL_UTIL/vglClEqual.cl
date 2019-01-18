/** Test if two images are equal.

    TODO: test limits when image is binary.

  */

#include "vglConst.h"

__kernel void vglClNdBinEqual(__global VGL_PACK_CL_SHADER_TYPE* img_input1,
			      __global VGL_PACK_CL_SHADER_TYPE* img_input2,
			      __global char* output)
{
  if(output[0] == 1) return;

#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

  VGL_PACK_CL_SHADER_TYPE p1 = img_input1[coord];
  VGL_PACK_CL_SHADER_TYPE p2 = img_input2[coord];

  if (!(p1 == p2))
  {
    output[0] = 1;
  }
}


__kernel void vglCl3dBinEqual(__read_only image3d_t img_input1,
			      __read_only image3d_t img_input2,
			      __global char* output)
{
    if(output[0] == 1) return;

    int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP |           //Clamp to zeros
                          CLK_FILTER_NEAREST;           //Don't interpolate
    float4 p1 = read_imagef(img_input1, smp, coords);
    float4 p2 = read_imagef(img_input2, smp, coords);
    if (!(p1.x == p2.x))
    {
        output[0] = 1;
    }
    if (!(p1.y == p2.y))
    {
        output[0] = 1;
    }
    if (!(p1.z == p2.z))
    {
        output[0] = 1;
    }
    if (!(p1.w == p2.w))
    {
        output[0] = 1;
    }
}


__kernel void vglClBinEqual(__read_only image2d_t img_input1,
			    __read_only image2d_t img_input2,
			    __global bool* output)
{
    if(output[0] == 1) return;

    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP |           //Clamp to zeros
                          CLK_FILTER_NEAREST;           //Don't interpolate
    float4 p1 = read_imagef(img_input1, smp, coords);
    float4 p2 = read_imagef(img_input2, smp, coords);
    if (!(p1.x == p2.x))
    {
        output[0] = 1;
    }
    if (!(p1.y == p2.y))
    {
        output[0] = 1;
    }
    if (!(p1.z == p2.z))
    {
        output[0] = 1;
    }
    if (!(p1.w == p2.w))
    {
        output[0] = 1;
    }
}

__kernel void vglCl3dEqual(__read_only image3d_t img_input1,__read_only image3d_t img_input2, __global char* output)
{
    if(output[0] == 1) return;

    int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP |           //Clamp to zeros
                          CLK_FILTER_NEAREST;           //Don't interpolate
    float4 p1 = read_imagef(img_input1, smp, coords);
    float4 p2 = read_imagef(img_input2, smp, coords);
    if (!(p1.x == p2.x))
    {
        //printf("x 3d NEQ! @ (%d, %d, %d): p1 = %f != %f = p2\n", coords.x, coords.y, coords.z, p1.x, p2.x);
        output[0] = 1;
    }
    if (!(p1.y == p2.y))
    {
        output[0] = 1;
    }
    if (!(p1.z == p2.z))
    {
        output[0] = 1;
    }
    if (!(p1.w == p2.w))
    {
        output[0] = 1;
    }
}
		
	
__kernel void vglClEqual(__read_only image2d_t img_input1,__read_only image2d_t img_input2, __global bool* output)
{
    if(output[0] == 1) return;

    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP |           //Clamp to zeros
                          CLK_FILTER_NEAREST;           //Don't interpolate
    float4 p1 = read_imagef(img_input1, smp, coords);
    float4 p2 = read_imagef(img_input2, smp, coords);
    if (!(p1.x == p2.x))
    {
        //printf("x NEQ! @ (%d, %d): p1 = %f != %f = p2\n", coords.x, coords.y, p1.x, p2.x);
        output[0] = 1;
    }
    if (!(p1.y == p2.y))
    {
        output[0] = 1;
    }
    if (!(p1.z == p2.z))
    {
        output[0] = 1;
    }
    if (!(p1.w == p2.w))
    {
        output[0] = 1;
    }
}
