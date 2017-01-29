/** Threshold of img_input by parameter. if the pixel is below thresh,
    the output is 0, else, the output is 1. Result is stored in img_output.
    Input image is 8bpp and output is 1bpp.
  */

// SHAPE in_shape  (img_input->vglShape->asVglClShape())
// SHAPE out_shape (img_output->vglShape->asVglClShape())

#include "vglClShape.h"

__kernel void vglClNdBinThreshold(__global char* img_input,
                                  __global char* img_output,
                                  unsigned char thresh,
                                  __constant VglClShape* in_shape,
                                  __constant VglClShape* out_shape)
{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

  int ws = out_shape->offset[VGL_SHAPE_HEIGHT];
  int w  = in_shape->shape[VGL_SHAPE_WIDTH];

  unsigned char result = 0;
  for (int bit = 0; bit < 8; bit++)
  {
    int j = (coord % ws) * 8 + 7 - bit;
    int i = (coord / ws);
    unsigned char p = img_input[(i * w) + j];
    unsigned char result_bit;
    if (p > thresh)
      result_bit = 1;
    else
      result_bit = 0;
    result += result_bit << bit;
  }

  img_output[coord] = result;

}
