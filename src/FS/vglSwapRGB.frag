
/** Convert image from RGB to BGR and vice versa.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){
  vec4 pix = texture2D(sampler0, gl_TexCoord[0].xy);
  gl_FragColor.rgb = pix.bgr; 

}
