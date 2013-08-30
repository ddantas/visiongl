
/** Stores sobel edge filtering in X direction in red channel
    grayscale in y and sobel edge filtering in Y direction in green channel

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

vec2 offset[] = { vec2(-1.0, -1.0),
                  vec2( 0.0, -1.0),
                  vec2( 1.0, -1.0),
                  vec2(-1.0,  0.0),
                  vec2( 0.0,  0.0),
                  vec2( 1.0,  0.0),
                  vec2(-1.0,  1.0),
                  vec2( 0.0,  1.0),
                  vec2( 1.0,  1.0),
                };

void main(void){
  vec4 pix[9];

  for (int i = 0; i < 9; i++){
    pix[i] = texture2D(sampler0, 
                       gl_TexCoord[0].xy + offset[i] / tex_size);
  }

  gl_FragColor.r = 
     abs         ( -     pix[0]              +     pix[2]
                   - 2.0*pix[3]              + 2.0*pix[5]
                   -     pix[6]              +     pix[8]  );

  gl_FragColor.g = dot(  vec3(.2125, .7154, .0721), 
                         texture2D(sampler0, gl_TexCoord[0].xy)  );
 

  gl_FragColor.b =
     abs         ( -     pix[0] - 2.0*pix[1] -     pix[2]

                   +     pix[3] + 2.0*pix[4] +     pix[5]   );

}
