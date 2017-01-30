/** Dilation of img_input by mask. Result is stored in img_output.

  */

// SHAPE img_shape (img_input->vglShape->asVglClShape())
// ARRAY convolution_window [window_size_x*window_size_y*window_size_z]
// SCALAR window_size_x
// SCALAR window_size_y

#include "vglClShape.h"

__kernel void vglCl3dBinDilate(__read_only image3d_t img_input,
                             __write_only image3d_t img_output,
                             __constant float* convolution_window, 
                             int window_size_x, 
                             int window_size_y,
                             int window_size_z,
                             __constant VglClShape* img_shape)
{
  int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
    
    int w_r = floor((float)window_size_x / 2.0f);
    int h_r = floor((float)window_size_y / 2.0f);
    int l_r = floor((float)window_size_z / 2.0f);
    int w_img = img_shape->shape[VGL_SHAPE_WIDTH];
    int h_img = img_shape->shape[VGL_SHAPE_HEIGHT];
    int l_img = img_shape->shape[VGL_SHAPE_LENGTH];
    int pad = (((w_r+h_r+l_r) / 8) + 1) * 8; // Avoids negative remainder.
    int i_raster;
    uint4 pmax;
    uint4 result = 0;
    for (int bit = 0; bit < 8; bit++)
    {
      pmax = 0;
      i_raster = 0; 
      for(int i_w = -h_r; i_w <= h_r && pmax.x == 0; i_w++)
      {
        for(int j_w = -w_r; j_w <= w_r && pmax.x == 0; j_w++)
        {
          for(int k_w = -l_r; k_w <= l_r && pmax.x == 0; k_w++)
          {
            int i_img = coords.y - i_w;
            int j_img = 8 * coords.x + 7 - bit - j_w;
            int k_img = coords.z - k_w;
            i_img = clamp(i_img, 0, h_img-1);
            j_img = clamp(j_img, 0, w_img-1);
            k_img = clamp(k_img, 0, l_img-1);

            uint4 p = read_imageui(img_input, smp, (int4)((j_img) / 8, i_img, k_img, 0));
            unsigned int result_bit;
            result_bit = p.x & (1 << ((pad + bit + j_w) % 8));

            if (!(convolution_window[i_raster] == 0))
              if (result_bit > 0)
                pmax.x = 1;

            i_raster++;
	  }
        }
      }
      result += pmax.x << bit;
    }
    write_imageui(img_output, coords, result);
}
