/** Dilation of img_input by mask. Result is stored in img_output.

  */

// SHAPE img_shape (img_input->vglShape->asVglClShape())
// ARRAY convolution_window [window_size_x*window_size_y]
// SCALAR window_size_x
// SCALAR window_size_y

#include "vglConst.h"
#include "vglClShape.h"

__kernel void vglClBinDilatePack(__read_only image2d_t img_input,
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
    int ws_img = img_shape->offset[VGL_SHAPE_HEIGHT] - 1;
    int i_l = 0;
    VGL_PACK_CL_SHADER_TYPE pad = VGL_PACK_MAX_UINT << (VGL_PACK_SIZE_BITS * img_shape->offset[VGL_SHAPE_HEIGHT] - img_shape->shape[VGL_SHAPE_WIDTH]);  // In erosion replace ( << ) with ( >> 8 - )
    VGL_PACK_CL_SHADER_TYPE boundary = 0;        // In erosion, 255
    VGL_PACK_CL_SHADER_TYPE result = 0;  // In erosion, 255

    // In erosion, create aux var here
    for(int i_w = -h_r; i_w <= h_r; i_w++)
    {
      for(int j_w = -w_r; j_w <= w_r; j_w++)
      {
        if (!(convolution_window[i_l] == 0))
	{
          int i_img = coords.y - i_w; // In erosion, replace - with +
          int j_img = coords.x;
          uint4 p; 
          if (j_w < 0)
          {
            p = read_imageui(img_input, smp, (int2)(j_img, i_img));
            if (j_img == ws_img)
              p.x = p.x & pad;  // In erosion, replace & with |
            result = result | (p.x >> ( -j_w));
            if (j_img == ws_img)
              p.x = boundary;
            else
              p = read_imageui(img_input, smp, (int2)(j_img + 1, i_img));
            result = result | (p.x << (VGL_PACK_SIZE_BITS+j_w));
          }
          else if (j_w > 0)
          {
            p = read_imageui(img_input, smp, (int2)(j_img, i_img));  // In erosion, place pad if in the next line
            result = result | (p.x << (  j_w));
            if (j_img == 0)
              p.x = boundary;
            else
              p = read_imageui(img_input, smp, (int2)(j_img - 1, i_img));
            result = result | (p.x >> (VGL_PACK_SIZE_BITS-j_w));
          }
          else
          {
            p = read_imageui(img_input, smp, (int2)(j_img, i_img));
            result = result | p.x;
          }
	}
        i_l++;
      }
    }
    //result = p.x >> 1; // 31 disappear
    //result = p.x << 1; //  0 disappear
    write_imageui(img_output, coords, result);
}
