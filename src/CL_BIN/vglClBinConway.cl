/** Conway game of life.

  */

__kernel void vglClBinConway(__read_only image2d_t img_input,
                             __write_only image2d_t img_output)
{
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
                          CLK_ADDRESS_CLAMP_TO_EDGE |   //Clamp to next edge
                          CLK_FILTER_NEAREST;           //Don't interpolate
    
    int wx = 1;
    int wy = 1;
    int wi;
    unsigned int neighbours;
    unsigned int alive;
    unsigned int result_bit;
    uint4 result = 0;
    for (int bit = 0; bit < 8; bit++)
    {
      neighbours = 0;
      alive = 0;
      result_bit = 0;
      wi = 0;
      for(int i = -wy; i <= wy; i++)
      {
        for(int j = -wx; j <= wx; j++)
        {
          uint4 p = read_imageui(img_input, smp, (int2)((8 * coords.x + 7 - bit + j) / 8, coords.y + i));
          unsigned int p_bit;
          p_bit = p.x & (1 << ((8 + bit - j) % 8));

          if (p_bit > 0)
	  {
            if (i == 0 && j == 0)
              alive = 1;
	    else
              neighbours++;
          }

          wi++;
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
