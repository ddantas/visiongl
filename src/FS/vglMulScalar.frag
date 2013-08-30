
/** Multiply image by scalar.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, float factor)

uniform sampler2D sampler0;
uniform float factor; //factor

void main(void){

  gl_FragColor = texture2D(sampler0, gl_TexCoord[0].st) * factor; 

}
