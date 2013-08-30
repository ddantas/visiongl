
/** Rescales corners of image to given corners

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, float x0, float y0, float x1, float y1)

uniform sampler2D src;

uniform float x_0;       // x0 / (float)src->width
uniform float y_0;       // y0 / (float)src->height
uniform float x_1;       // x1 / (float)src->width
uniform float y_1;       // y1 / (float)src->height

void main(void){

  float x_d = gl_TexCoord[0].x;
  float y_d = gl_TexCoord[0].y;

  float x_src = (x_d - x_0) / (x_1 - x_0);
  float y_src = (y_d - y_0) / (y_1 - y_0);

  if ( x_src >= 0.0 && y_src >= 0.0 && 
       x_src <= 1.0 && y_src <= 1.0     ){
    gl_FragColor = texture2D(src, vec2(x_src, y_src));
  }
  else{
    gl_FragColor = 0.0;
  }


}
