
/** Roberts gradient of image

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
                };

void main(void){
  vec4 pix[9];
  vec4 x, y;

  for (int i = 0; i < 9; i++){
    pix[i] = texture2D(sampler0, 
                       gl_TexCoord[0].xy + offset[i] / tex_size);
  }

  x            = (                           -     pix[2]
                                +     pix[4]                );

  y            = ( -     pix[0]                            
                                +     pix[4]                );

  gl_FragColor = sqrt(x*x + y*y) / 2.0;

}
