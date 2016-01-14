
/** Convert grayscale image to RGB

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;
uniform int width; // src->getWidth()

void main(void){
  vec4 pix = texture2D(sampler0, vec2(gl_TexCoord[0].s / 3.0, gl_TexCoord[0].t / 3.0)); 
  float  x   = gl_TexCoord[0].x * width / 3.0;
  float  m   = mod(x, 3.0);

  if (m < 1.0)
    gl_FragColor.rgb = pix.r;
  else if (m < 2.0)
    gl_FragColor.rgb = pix.g;
  else if (m < 3.0)
    gl_FragColor.rgb = pix.b;
  else
    gl_FragColor.rgb = 1.0;

  //gl_FragColor = pix;

}
