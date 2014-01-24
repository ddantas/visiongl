/** Convolution of src image by mask. Result is stored in dst image.

  */

__kernel void vglClBlurSq3(__read_only image2d_t img_input,__write_only image2d_t img_output)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate

    float4 result = (0,0,0,0);
    float convolution_window[9] = {1.0/16.0, 2.0/16.0, 1.0/16.0, 
                                   2.0/16.0, 4.0/16.0, 2.0/16.0, 
                                   1.0/16.0, 2.0/16.0, 1.0/16.0, };

    int xi = 0;
    int yi = 0;
    for(xi = -1; xi <= 1; xi++)
    {
        for(yi = -1; yi <= 1; yi++)
        {
            float4 p = read_imagef(img_input, smp, (int2)(coords.x + xi,coords.y + yi));
            p.xyz *= convolution_window[3 * (xi+1) + yi + 1];
            result += p;
	}
    }
    write_imagef(img_output, coords, result);
}
