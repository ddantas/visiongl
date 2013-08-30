
/** Erosion of image by 7x7 square structuring element.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

void main(void){
  vec4 pix;
  vec4 chosen_val;

  chosen_val = vec4(1.0);

  for (int i = -3; i <= 3; i++){
    for (int j = -3; j <= 3; j++){
      pix = texture2D(sampler0, 
                      gl_TexCoord[0].xy + vec2(i, j) / tex_size);
      chosen_val = min(pix, chosen_val);
    }
  }
  gl_FragColor = chosen_val;
}
