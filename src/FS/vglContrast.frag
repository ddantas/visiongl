
/** Changes contrast of image by given factor.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, float factor)

uniform sampler2D sampler0;
uniform float factor; // factor

void main(void){
  vec3 pix;

  pix = texture2D(sampler0, gl_TexCoord[0].xy);

  pix = (((pix - .5) * factor) + .5); 

  gl_FragColor = vec4(pix, 1.);

}
