/** Copy N-dimensional image word by word.

  */

#include "vglConst.h"

__kernel void vglClNdBinCopy(__global VGL_PACK_CL_SHADER_TYPE* img_input, __global VGL_PACK_CL_SHADER_TYPE* img_output)
{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

  img_output[coord] = img_input[coord];

}
