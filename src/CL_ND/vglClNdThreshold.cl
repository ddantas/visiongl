/** Threshold of img_input by parameter. if the pixel is below thresh,
    the output is 0, else, the output is top. Result is stored in img_output.
  */

__kernel void vglClNdThreshold(__global char* img_input,
                          __global char* img_output,
                          unsigned char thresh,
                          unsigned char top /*= 255*/)
{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

  img_output[coord] = img_input[coord];

  if( img_input[coord] > thresh)
    img_output[coord] = top;
  else
    img_output[coord] = 0;

}
