
/** Sobel edge filtering in Y direction.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

vec2 offset[] = { vec2(-1.0, -1.0),
                  vec2( 0.0, -1.0),
                  vec2( 1.0, -1.0),
//                vec2(-1.0,  0.0),
//                vec2( 0.0,  0.0),
//                vec2( 1.0,  0.0),
                  vec2(-1.0,  1.0),
                  vec2( 0.0,  1.0),
                  vec2( 1.0,  1.0),
                };

void main(void){
  vec4 pix[6];

  for (int i = 0; i < 6; i++){
    pix[i] = texture2D(sampler0, 
                       gl_TexCoord[0].xy + offset[i] / tex_size);
  }

  gl_FragColor =
     abs         ( -     pix[0] - 2.0*pix[1] -     pix[2]

                   +     pix[3] + 2.0*pix[4] +     pix[5]   );

}
