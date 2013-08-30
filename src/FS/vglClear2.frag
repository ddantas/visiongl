
/** Clear image with given color.

  */

// (IN_OUT: VglImage* src_dst, float r, float g, float b, float a = 0.0)

uniform sampler2D sampler0;
uniform float r; //r
uniform float g; //g
uniform float b; //b
uniform float a; //a

void main(void){
  vec4 pix;

  pix.r = r;
  pix.g = g;
  pix.b = b;
  pix.a = a;

  gl_FragData[0] = pix;

}
