
/** Direct copy from src to dst.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){

  gl_FragColor = texture2D(sampler0, gl_TexCoord[0].st); 

}
