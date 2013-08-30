
/** Initialize image to be used in baricenter calculation. The initialization is done by storing the values (1, x, y) in each output pixel so that the summation over th whole image gives the three moments of the image.

    R =                    f(x, y) 

    G =                x * f(x, y) 

    B =                y * f(x, y) 

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

void main(void){
  vec4 input;
  vec4 output;

  input = texture2D(sampler0, gl_TexCoord[0].xy);

  if(input.r > 0.0 || input.g > 0.0 || input.b > 0.0){
    output.r = 1.0;
    output.gb = floor(gl_TexCoord[0].xy * tex_size);
  }
  else{
    output = 0.0;
  }

  gl_FragColor = output;

}
