
/** Test and model for IN_OUT semantics, with double output.

  */

// (IN_OUT: VglImage* src_dst, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){

  if ( length( texture2D(sampler0, gl_TexCoord[0].xy) ) < .5){
    gl_FragData[0] = 1.0 - texture2D(sampler0, gl_TexCoord[0].xy) ;
  }
  else{
    gl_FragData[0] = texture2D(sampler0, gl_TexCoord[0].xy) ;
  }
  gl_FragData[1] =  1.5 * texture2D(sampler0, gl_TexCoord[0].xy) ;

}
