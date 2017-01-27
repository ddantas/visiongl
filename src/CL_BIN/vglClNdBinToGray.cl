/** Convert binary image to grayscale.

    Convert binary image to grayscale.
  */

// SHAPE in_shape  (img_input->vglShape->asVglClShape())
// SHAPE out_shape (img_output->vglShape->asVglClShape())

#include "vglClShape.h"

__kernel void vglClNdBinToGray(__global char* img_input,
                               __global char* img_output,
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

  int ws = in_shape->offset[VGL_SHAPE_HEIGHT];
  int w  = out_shape->shape[VGL_SHAPE_WIDTH];

  if (coord == 0)
    printf("ws = %d, w = %d\n", ws, w);

  unsigned char result = 0;
  unsigned char p = img_input[coord];
  for (int bit = 0; bit < 8; bit++)
  {
    unsigned char result_bit = p & (1 << bit);
    if (result_bit)
      result = 255;
    else
      result = 0;
    int j = (coord % ws) * 8 + 7 - bit;
    int i = (coord / ws);
    if (j < w)
      img_output[(i * w) + j] = result;
  }
}
