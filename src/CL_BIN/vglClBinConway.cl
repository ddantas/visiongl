/** Conway game of life.

  */

// SHAPE img_shape (img_input->vglShape->asVglClShape())

#include "vglClShape.h"

__kernel void vglClBinConway(__read_only image2d_t img_input,
                             __write_only image2d_t img_output,
                             __constant VglClShape* img_shape)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
    
    int w_r = 1;
    int h_r = 1;
    int w_img = img_shape->shape[VGL_SHAPE_WIDTH];
    int h_img = img_shape->shape[VGL_SHAPE_HEIGHT];
    int i_l;
    unsigned int neighbours;
    unsigned int alive;
    unsigned int result_bit;
    uint4 result = 0;
    for (int bit = 0; bit < 8; bit++)
    {
      neighbours = 0;
      alive = 0;
      result_bit = 0;
      i_l = 0;
      for(int i_w = -h_r; i_w <= h_r; i_w++)
      {
        for(int j_w = -w_r; j_w <= w_r; j_w++)
        {
          int i_img = coords.y + i_w;
          int j_img = 8 * coords.x + 7 - bit + j_w;
          uint4 p;
          unsigned int p_bit = 0;
 
          if ( (i_img >= 0) && (i_img < h_img) && (j_img >= 0) && (j_img < w_img) )
	  {
            p = read_imageui(img_input, smp, (int2)(j_img / 8, i_img));
            p_bit = p.x & (1 << ((8 + bit - j_w) % 8));
          }

          if (p_bit > 0)
	  {
            if (i_w == 0 && j_w == 0)
              alive = 1;
	    else
              neighbours++;
          }

          i_l++;
        }
      }

      if (    ( (alive > 0)  && (neighbours == 2 || neighbours == 3) )    ||
              ( (alive == 0) && (neighbours == 3) )    )
      {
        result_bit = 1;
      }

      result += result_bit << bit;
    }
    write_imageui(img_output, coords, result);
}
