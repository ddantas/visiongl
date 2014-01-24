
/** vglBlurSq3

    Blur image by 3x3 square structuring element.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

vec2 offset[] = { vec2(-1.0, -1.0),
                  vec2( 0.0, -1.0),
                  vec2( 1.0, -1.0),
                  vec2(-1.0,  0.0),
                  vec2( 0.0,  0.0),
                  vec2( 1.0,  0.0),
                  vec2(-1.0,  1.0),
                  vec2( 0.0,  1.0),
                  vec2( 1.0,  1.0),
                };

void main(void){
  vec4 pix[9];

  for (int i = 0; i < 9; i++){
    pix[i] = texture2D(sampler0, 
                       gl_TexCoord[0].xy + offset[i] / tex_size);
    //chosen_val = min(pix[i], chosen_val);
  }

  gl_FragColor = ( +     pix[0] + 2.0*pix[1] +     pix[2]
                   + 2.0*pix[3] + 4.0*pix[4] + 2.0*pix[5]
                   +     pix[6] + 2.0*pix[7] +     pix[8]  ) / 16.0;

}
