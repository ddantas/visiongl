
/** VglSumDiff

    Sum of differences

  */

// (IN_TEX: VglImage* img_dif, OUT_FBO: VglImage* img_out)

uniform sampler2D img_dif;
uniform vec2 tex_size; // img_dif->ipl->width, img_dif->ipl->height

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
    pix[i] = texture2D(img_dif, 
                       gl_TexCoord[0].xy + offset[i] / tex_size);
  }

  gl_FragColor = ( +     pix[0] +     pix[1] +     pix[2]
                   +     pix[3] +     pix[4] +     pix[5]
                   +     pix[6] +     pix[7] +     pix[8]   )  /  9.0;


}
