
/** Convert image to grayscale by calculating the scalar product of (r, g, b) and (.2125, .7154, .0721).

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){

  gl_FragColor = dot(  vec3(.2125, .7154, .0721), 
                       texture2D(sampler0, gl_TexCoord[0].xy)  );

}
