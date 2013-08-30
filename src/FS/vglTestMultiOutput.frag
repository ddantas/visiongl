
/** Test and model for multiple output functions.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, OUT_FBO: VglImage* dst1)

uniform sampler2D sampler0;

void main(void){

  gl_FragData[0] = .5   * texture2D(sampler0, gl_TexCoord[0].xy) ;

  gl_FragData[1] =  2.0 * texture2D(sampler0, gl_TexCoord[0].xy) ;

}
