/** Dilation of img_input by mask. Result is stored in img_output.

  */

// SHAPE img_shape (img_input->vglShape->asVglClShape())
// ARRAY convolution_window [window_size_x*window_size_y*window_size_z]
// SCALAR window_size_x
// SCALAR window_size_y
// SCALAR window_size_z

#include "vglClShape.h"

__kernel void vglCl3dBinDilatePack(__read_only image3d_t img_input,
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
    int ws_img = img_shape->offset[VGL_SHAPE_HEIGHT] - 1;
    int i_l = 0;
    uint4 boundary = 0;        // In erosion, 255
    unsigned char result = 0;  // In erosion, 255
    for(int k_w = -l_r; k_w <= l_r; k_w++)
    {
      for(int i_w = -h_r; i_w <= h_r; i_w++)
      {
        for(int j_w = -w_r; j_w <= w_r; j_w++)
        {
          if (!(convolution_window[i_l] == 0))
          {
            int k_img = coords.z - k_w; // In erosion, replace - with +
            int i_img = coords.y - i_w; // In erosion, replace - with +
            int j_img = coords.x;
            uint4 p; 
            if (j_w < 0)
            {
              p = read_imageui(img_input, smp, (int4)(j_img, i_img, k_img, 0));
              result = result | (p.x << ( -j_w));
              if (j_img == ws_img)
                p = boundary;
              else
                p = read_imageui(img_input, smp, (int4)(j_img + 1, i_img, k_img, 0));
              result = result | (p.x >> (8+j_w));
            }
            else if (j_w > 0)
            {
              p = read_imageui(img_input, smp, (int4)(j_img, i_img, k_img, 0));
              result = result | (p.x >> (  j_w));
              if (j_img == 0)
                p = boundary;
              else
                p = read_imageui(img_input, smp, (int4)(j_img - 1, i_img, k_img, 0));
              result = result | (p.x << (8-j_w));
            }
            else
            {
              p = read_imageui(img_input, smp, (int4)(j_img, i_img, k_img, 0));
              result = result | p.x;
            }
          }
          i_l++;
	}
      }
    }
    write_imageui(img_output, coords, result);
}
