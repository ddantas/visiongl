/** N-dimensional dilation

    SHAPE directive passes a structure with size of each dimension, offsets and number of dimensions. Parameter does not appear in wrapper parameter list. The C expression between parenthesis returns the desired shape of type VglClShape.
    
  */

//SHAPE img_shape (img_input->vglShape->asVglClShape())

#include "vglClShape.h"
#include "vglClStrEl.h"

__kernel void vglClNdBinDilatePack(__global unsigned char* img_input, 
                                   __global unsigned char* img_output,  
                                   __constant VglClShape* img_shape,
                                   __constant VglClStrEl* window)
{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

  int ires;
  int idim;
  ires = coord;
  int img_coord[VGL_ARR_SHAPE_SIZE];
  //int img_limit[VGL_ARR_SHAPE_SIZE];
  //int img_bit_coord;
  int win_coord[VGL_ARR_SHAPE_SIZE];
  int win_radius[VGL_ARR_SHAPE_SIZE];
  int j_img;
  int ws_img;

  for(int d = img_shape->ndim; d >= 1; d--)
  {
    int off = img_shape->offset[d];
    idim = ires / off;
    ires = ires - idim * off;

    if (d == VGL_SHAPE_WIDTH)
    {
      //img_coord[d] = idim + ((window->shape[d] - 1) / 2); //In erosion, replace + with -
      //img_limit[d] = off;
      j_img = idim;
    }
    else
    {
      //img_coord[d] = idim + ((window->shape[d] - 1) / 2); //In erosion, replace + with -
      //img_limit[d] = img_shape->shape[d];
    }
    win_radius[d] = ((window->shape[d] - 1) / 2);
    img_coord[d] = idim;
  }

  ws_img = img_shape->offset[VGL_SHAPE_HEIGHT] - 1;

  //if (coord == 0)
  //  printf("ws_img = %d\n", ws_img);
  //printf("coord = %6d j_img = %6d\n", coord, j_img);

  unsigned char pad = 255 << 8 * img_shape->offset[VGL_SHAPE_HEIGHT] - img_shape->offset[VGL_SHAPE_WIDTH];
  unsigned char boundary = 0;  // In erosion, 255
  unsigned char result = 0;    // In erosion, 255
  for(int i = 0; i < window->size; i++)
  {
    //int j_bit;
    //int j_byte;
    int conv_coord;
    if (!(window->data[i] == 0))
    {
      ires = i;
      conv_coord = 0;
      for(int d = img_shape->ndim; d > window->ndim; d--)
      {
        conv_coord += img_shape->offset[d] * img_coord[d];
      }
      for(int d = window->ndim; d >= 1; d--)
      {
        int off = window->offset[d];
        idim = ires / off;
        ires = ires - idim * off;

        if (d == VGL_SHAPE_WIDTH)
	{
          win_coord[d] = img_coord[d];                          //In erosion, remove the -
        }
        else
        {
          win_coord[d] = - idim + img_coord[d] + win_radius[d]; //In erosion, remove the -
          win_coord[d] = clamp(win_coord[d], 0, img_shape->shape[d]-1);
	}
        conv_coord += img_shape->offset[d] * win_coord[d];


        if (d == VGL_SHAPE_WIDTH)
        {
            int j_w = idim - win_radius[VGL_SHAPE_WIDTH];
            unsigned char p; 
            if (j_w < 0)
            {
              p = img_input[conv_coord];
              if (j_img == ws_img)
                p = p & pad;
              result = result | (p << ( -j_w));
              if (j_img == ws_img)
                p = boundary;
              else
                p = img_input[conv_coord + 1];
              result = result | (p >> (8+j_w));
            }
            else if (j_w > 0)
            {
              p = img_input[conv_coord];
              result = result | (p >> (  j_w));
              if (j_img == 0)
                p = boundary;
              else
                p = img_input[conv_coord - 1];
              result = result | (p << (8-j_w));
            }
            else
            {
              p = img_input[conv_coord];
              result = result | p;
            }

	}
        else
	{
          //win_coord[d] = - idim + img_coord[d] + win_radius[d]; //In erosion, remove the -
          //win_coord[d] = clamp(win_coord[d], 0, img_shape->shape[d]-1);
	}
        //conv_coord += img_shape->offset[d] * win_coord[d];
      }
    }
  }
  img_output[coord] = result;
}
