
/** Erosion of image by horizontal line with 7 pixels.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

vec2 offset[] = {
                  vec2(-3.0,  0.0),
                  vec2(-2.0,  0.0),
                  vec2(-1.0,  0.0),
                  vec2( 0.0,  0.0),
                  vec2( 1.0,  0.0),
                  vec2( 2.0,  0.0),
                  vec2( 3.0,  0.0),
                };

void main(void){
  vec4 pix;
  vec4 chosen_val;

  chosen_val = vec4(1.0);

  for (int i = 0; i < 7; i++){
    pix = texture2D(sampler0, 
                       gl_TexCoord[0].xy + offset[i] / tex_size);
    chosen_val = min(pix, chosen_val);
  }
  gl_FragColor = chosen_val;
}
