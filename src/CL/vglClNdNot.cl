/** Direct copy from src to dst.

  */

__kernel void vglClNdNot(__global char* img_input, __global char* img_output)
{
    int x = get_global_id(0);
    int y = get_global_id(1);
    int w = get_global_size(0);
    int coord = y*w+x;

    img_output[coord] = img_input[coord];

}


/*
__kernel void vglClNdNot(__read_only image3d_t img_input, __write_only image3d_t img_output)
{
  int4 coords = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
  write_imagef(img_output, coords, get_global_id(2) /17.0);
}
*/
