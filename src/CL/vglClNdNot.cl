/** Direct copy from src to dst.

  */

__kernel void vglClNdNot(__read_only char* img_input, __write_only char* img_output)
{
	int coord = get_global_id(0);
	
	img_output[coord]=255-img_input[coord];
}
