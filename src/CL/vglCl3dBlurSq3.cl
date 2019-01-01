/** Convolution of src image by mask. Result is stored in dst image.
    
    In some OpenCL versions, the next directive is required
    #pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

  */


__kernel void vglCl3dBlurSq3(__read_only image3d_t img_input, __write_only image3d_t img_output)
{
    int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate

    float4 result = (0,0,0,0);
    float convolution_window[27] = {1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
                                    1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
                                    1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
                                    1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f,
                                    1.0f/16.0f, 1.0f/8.0f, 1.0f/16.0f, 
                                    1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
                                    1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
                                    1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
                                    1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f};

    int xi = 0;
    int yi = 0;
    int zi = 0;
    for(zi = 0; zi < 3; zi++)
    {
        for(yi = 0; yi < 3; yi++)
        {
            for(xi = 0; xi < 3; xi++)
            {
                float4 p = read_imagef(img_input, smp, (int4)(coords.x + (xi-1),coords.y + (yi-1), coords.z + (zi-1), 0));
                p.xyz *= convolution_window[(zi*9) + (yi*3) + xi];
                result += p;
            }
        }
    }
    write_imagef(img_output, coords, result);
}
