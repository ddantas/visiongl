
/** Updates average and variance of background model.

  */

// (IN_TEX: VglImage* img_in, IN_OUT: VglImage* average, IN_OUT: VglImage* variance, float weight)

uniform sampler2D img_in;
uniform sampler2D img_avg;
uniform sampler2D img_var;

uniform float weight; //weight

void main(void){

  vec4 val = texture2D(img_in,  gl_TexCoord[0].st);
  vec4 avg = texture2D(img_avg, gl_TexCoord[0].st);
  vec4 var = texture2D(img_var, gl_TexCoord[0].st);

  vec4 delta = val - avg;

  gl_FragData[0] = avg + weight * delta;
  gl_FragData[1] = var + weight * (delta*delta - var);

}
