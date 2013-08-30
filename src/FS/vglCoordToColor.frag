
/** Shows coordinates of pixels as colors. Red is horizontal and green is vertical. Coordinates and colors are defined by OpenGL, that is, between 0 and 1.

  */

// (OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){

  //gl_FragColor.rg =  vec2(-.5, -.5) + 2.0 * gl_TexCoord[0].xy;
  gl_FragColor.rg =  gl_TexCoord[0].xy;

}
