/** Generate ROI.

    Generate ROI (Region Of Interest). Useful to be used as mask to do intersection
    with other images.

  */

// ARRAY p0 [VGL_ARR_SHAPE_SIZE]
// ARRAY pf [VGL_ARR_SHAPE_SIZE]
// SHAPE img_shape (img_output->vglShape->asVglClShape())

#include "vglClShape.h"

__kernel void vglClNdBinRoi(__global VGL_PACK_CL_SHADER_TYPE* img_output,
                            __constant int* p0,
                            __constant int* pf,
                            __constant VglClShape* img_shape)

{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

  VGL_PACK_OUTPUT_SWAP_MASK

  VGL_PACK_CL_SHADER_TYPE result = 0;
  VGL_PACK_CL_SHADER_TYPE in_roi = 1;
  int ires = coord;
  int idim;

  for (int d = img_shape->ndim; d >= VGL_SHAPE_WIDTH && in_roi > 0; d--)
  {
    int off = img_shape->offset[d];
    idim = ires / off;
    ires = ires - idim * off;

    if (  (!(d == VGL_SHAPE_WIDTH))  &&  ( (idim < p0[d]) || (idim > pf[d]) )  )
    {
      in_roi = 0;
    }
  }

  int j_word = idim;
  for (int bit = 0; bit < VGL_PACK_SIZE_BITS && in_roi > 0; bit++)
  {
    int j_bit = j_word * VGL_PACK_SIZE_BITS + bit;
    if ( (j_bit >= p0[VGL_SHAPE_WIDTH]) && (j_bit <= pf[VGL_SHAPE_WIDTH]) )
    {
      result = result | outputSwapMask[bit];
    }
  }
  img_output[coord] = result;
}
