
/** Finds edge by using a White-Rohrer mask.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, float radius)

uniform sampler2D sampler0;

uniform vec2  tex_size; // src->ipl->width, src->ipl->height
uniform float radius;   // radius

vec2 offset[] = { vec2( 0.0,  0.0),
                  vec2( 0.0, -1.0),
                  vec2(-1.0,  0.0),
                  vec2( 0.0,  1.0),
                  vec2( 1.0,  0.0),
                };

void main(void){
  vec4 pix[9];

  for (int i = 0; i < 5; i++){
    pix[i] = texture2D(sampler0, 
                       gl_TexCoord[0].xy + (radius * offset[i]) / tex_size);
  }

  gl_FragColor = (              +     pix[4]             
                   +     pix[1] - 4.0*pix[0] +     pix[3]
                                +     pix[2]               );

}
