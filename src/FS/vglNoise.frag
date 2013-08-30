
/** Add gaussian noise to image

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){

  if (noise1(gl_TexCoord[0].xy) > 0)
    gl_FragColor.r = 1.0;
  else
    gl_FragColor.r = 0.0;
  gl_FragColor.g = dFdy(gl_TexCoord[0].xy);
  gl_FragColor.b = 1.0;

}
