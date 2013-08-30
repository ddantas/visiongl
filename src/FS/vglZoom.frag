
/** Zoom image by factor.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, float factor)

uniform sampler2D sampler0;
uniform float factor; // factor

void main(void){

  gl_FragColor = texture2D( sampler0, 
                            vec2( (gl_TexCoord[0].s - .5) / factor + .5, 
                                  (gl_TexCoord[0].t - .5) / factor + .5) ); 
 
}
