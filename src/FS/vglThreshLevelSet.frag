
/** Threshold of image. If value is equal to level, output is top,
    else, output is 0. Default top value is 1.  
    Use after some Distance Transform to get a single distance level set.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, float thresh, float top = 1.0)

uniform sampler2D sampler0;
uniform float thresh; // thresh
uniform float top; // top

void main(void){
  vec3 pix;

  pix = texture2D(sampler0, gl_TexCoord[0].xy);

  /*
  if (pix.r > thresh)
    gl_FragColor.r = 1.0;
  else 
    gl_FragColor.r = 0.0;

  if (pix.g > thresh)
    gl_FragColor.g = 1.0;
  else 
    gl_FragColor.g = 0.0;

  if (pix.b > thresh)
    gl_FragColor.b = 1.0;
  else 
    gl_FragColor.b = 0.0;
  */

    vec3 color = texture2D(sampler0, gl_TexCoord[0].st).rgb;

    if (color.r == thresh) color.r = top;
    else color.r = 0.;

    if (color.g == thresh) color.g = top;
    else color.g = 0.;

    if (color.b == thresh) color.b = top;
    else color.b = 0.;

	    gl_FragColor = vec4(color, 1.);

}
