
/** Inverts image.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){

  gl_FragColor = 1.0-texture2D(sampler0, gl_TexCoord[0].xy); 

}
