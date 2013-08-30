
/** Flip image vertically i.e. top becomes bottom.

    Image flip done by shader.
  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){

  gl_FragColor = texture2D(sampler0, 
                           vec2(gl_TexCoord[0].s, 1.0-gl_TexCoord[0].t)); 
 
}
