/** Dilation of img_input by mask. Result is stored in img_output.

  */

// ARRAY convolution_window [window_size_x*window_size_y]
// SCALAR window_size_x
// SCALAR window_size_y

__kernel void vglClBinErode(__read_only image2d_t img_input,
                             __write_only image2d_t img_output,
                             __constant float* convolution_window, 
                             int window_size_x, 
                             int window_size_y)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
    
    int wx = floor((float)window_size_x / 2.0f);
    int wy = floor((float)window_size_y / 2.0f);
    int pad = ((wx / 8) + 1) * 8; // Avoids negative remainder.
    int wi;
    uint4 pmin;
    uint4 result = 0;
    for (int bit = 0; bit < 8; bit++)
    {
      pmin = 1;
      wi = 0; 
      for(int i = -wy; i <= wy; i++)
      {
        for(int j = -wx; j <= wx; j++)
        {
          uint4 p = read_imageui(img_input, smp, (int2)((8 * coords.x + 7 - bit + j) / 8, coords.y + i));
          unsigned int result_bit;
          result_bit = p.x & (1 << ((pad + bit - j) % 8));

          if (!(convolution_window[wi] == 0))
            if (result_bit == 0)
              pmin.x = 0;

          wi++;
        }
      }
      result += pmin.x << bit;
    }
    write_imageui(img_output, coords, result);
}
