
/** Erosion of image by square structuring element. The parameter "side" is the dimension of the square side in pixels.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, int side)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height
uniform int radius; // (side - 1) / 2

void main(void){
  vec4 pix;
  vec4 chosen_val;

  chosen_val = vec4(1.0);

  for (int i = -radius; i <= radius; i++){
    for (int j = -radius; j <= radius; j++){
      pix = texture2D(sampler0, 
                      gl_TexCoord[0].xy + vec2(i, j) / tex_size);
      chosen_val = min(pix, chosen_val);
    }
  }
  gl_FragColor = chosen_val;
}
