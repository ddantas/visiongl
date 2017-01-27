/** Negation of binary image img_input. Result is stored in img_output.

  */

__kernel void vglClNdBinNot(__global char* img_input,
                            __global char* img_output)
{
#if __OPENCL_VERSION__ < 200
  int coord = (  (get_global_id(2) - get_global_offset(2)) * get_global_size(1) * get_global_size(0)) +
              (  (get_global_id(1) - get_global_offset(1)) * get_global_size (0)  ) +
                 (get_global_id(0) - get_global_offset(0));
#else
  int coord = get_global_linear_id();
#endif

  unsigned char p = img_input[coord];
  img_output[coord] = 0xff & ~p;
}
