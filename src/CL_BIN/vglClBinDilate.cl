/** Dilation of img_input by mask. Result is stored in img_output.

  */

// SHAPE img_shape (img_input->vglShape->asVglClShape())
// ARRAY convolution_window [window_size_x*window_size_y]
// SCALAR window_size_x
// SCALAR window_size_y

#include "vglClShape.h"

__kernel void vglClBinDilate(__read_only image2d_t img_input,
                             __write_only image2d_t img_output,
                             __constant float* convolution_window, 
                             int window_size_x, 
                             int window_size_y,
                             __constant VglClShape* img_shape)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate

    int w_r = floor((float)window_size_x / 2.0f);
    int h_r = floor((float)window_size_y / 2.0f);
    int w_img = img_shape->shape[VGL_SHAPE_WIDTH];
    int h_img = img_shape->shape[VGL_SHAPE_HEIGHT];
    int pad = 32;// ((w_r / VGL_PACK_SIZE_BITS) + 1) * VGL_PACK_SIZE_BITS; // Avoids negative remainder.
    int i_l;
    uint4 pmax;  // In erosion, replace pmax with pmin
    uint4 result = 0;
    for (int bit = 0; bit < VGL_PACK_SIZE_BITS; bit++)
    {
      pmax = 0;  // In erosion, replace 0 with 1
      i_l = 0; 
      for(int i_w = -h_r; i_w <= h_r && pmax.x == 0; i_w++)
      {
        for(int j_w = -w_r; j_w <= w_r && pmax.x == 0; j_w++)
        {
          int i_img = coords.y - i_w;                             // In erosion, replace - with +
          int j_img = VGL_PACK_SIZE_BITS * coords.x + bit - j_w;  // In erosion, replace - j_w with + j_w
          i_img = clamp(i_img, 0, h_img-1);
          j_img = clamp(j_img, 0, w_img-1);

          int j_img_word = (j_img) / VGL_PACK_SIZE_BITS;
          uint4 p = read_imageui(img_input, smp, (int2)(j_img_word, i_img));
          unsigned int result_bit;
          result_bit = p.x & (1 << (j_img - j_img_word * VGL_PACK_SIZE_BITS));  // In erosion, replace + j_w with - j_w

          if (!(convolution_window[i_l] == 0))
            if (result_bit > 0)  // In erosion, replace > with ==
              pmax.x = 1;

          i_l++;
        }
      }
      result += pmax.x << bit;
    }
    write_imageui(img_output, coords, result);
}
