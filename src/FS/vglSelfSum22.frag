
/** Stores in output pixel the sum of 4 adjacent pixels of the input
    image. 
    The width and height of the output image must be half of the input image.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

void main(void){
  vec4 pix;
  vec4 chosen_val;

  chosen_val = vec4(0.0);

  for (int i = 0; i < 2; i++){
    for (int j = 0; j < 2; j++){
      pix = texture2D(sampler0, 
                      gl_TexCoord[0].xy + vec2(j - .5, i - .5) / tex_size);
      chosen_val += pix;
    }
  }
  gl_FragColor = chosen_val / 1.0;

}
