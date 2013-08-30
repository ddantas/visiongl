
/** Detects foreground pixels.

  */

// (IN_TEX: VglImage* img_in, IN_TEX: VglImage* average, IN_TEX: VglImage* variance, OUT_FBO: VglImage* foreground, float std_thresh)

uniform sampler2D img_in;
uniform sampler2D img_avg;
uniform sampler2D img_var;

uniform float std_thresh; //std_thresh

void main(void){

  vec4 val = texture2D(img_in,  gl_TexCoord[0].st);
  vec4 avg = texture2D(img_avg, gl_TexCoord[0].st);
  vec4 var = texture2D(img_var, gl_TexCoord[0].st);
  vec3 delta = vec3(val - avg);

  float var_threshold = var.r + var.g + var.b;
  //float var_threshold = dot(sqrt(var));
  float sum_d2 = dot(delta, delta);
  var_threshold = std_thresh*std_thresh*var_threshold;

  if (sum_d2 <= var_threshold){
    gl_FragColor = 0.0;    
  }
  else{
    gl_FragColor = 1.0;
  }

}
