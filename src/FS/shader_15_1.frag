
// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

uniform float width;   // src->ipl->width
uniform float height;  // src->ipl->height

/*
const float DX = 1.0/640;
const float DY = 1.0/480;
vec2 offset[] = { vec2( -DX,  -DY),
                  vec2( 0.0,  -DY),
                  vec2(  DX,  -DY),
                  vec2( -DX,  0.0),
                  vec2( 0.0,  0.0),
                  vec2(  DX,  0.0),
                  vec2( -DX,   DX),
                  vec2( 0.0,   DX),
                  vec2(  DX,   DX),
                };
*/

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

  //gl_FragColor = mix(gl_Color, vec4(vec3(gl_SecondaryColor), 0.0), 0.5);
  //gl_FragColor = gl_TexCoord[0];
  
  // Direct texture mapping
  gl_FragColor = texture2D(sampler0, gl_TexCoord[0].st); 

  /* Horizontal flip
  gl_FragColor = texture2D(sampler0, 
                           vec2(-gl_TexCoord[0].s, gl_TexCoord[0].t)); 
  /**/

  /* Vertical flip
  gl_FragColor = texture2D(sampler0, 
                           vec2(gl_TexCoord[0].s, -gl_TexCoord[0].t)); 
  /**/

  /*/ // Negative
  gl_FragColor = 1.0 - texture2D(sampler0, gl_TexCoord[0].xy); 
  /**/

  // Filters
  vec4 pix[9];
  //vec4 chosen_val;

  //chosen_val = vec4(0.0);  // Dilation
  //chosen_val = vec4(1.0);  // Erosion

  for (int i = 0; i < 9; i++){
    pix[i] = texture2D(sampler0, 
                       gl_TexCoord[0].xy + offset[i] / tex_size);

    /*/ Dilate by square
    chosen_val = max(pix[i], chosen_val);
    /**/ 
    // Erode by square
    //chosen_val = min(pix[i], chosen_val);

  }
  //gl_FragColor = chosen_val;

  /**/

  /*/ // Blur
  gl_FragColor = ( +     pix[0] + 2.0*pix[1] +     pix[2]
                   + 2.0*pix[3] +     pix[4] + 2.0*pix[5]
                   +     pix[6] + 2.0*pix[7] +     pix[8]  ) / 13.0;
  /**/

  /*/ Gaussian Blur
  gl_FragColor = ( +     pix[0] + 2.0*pix[1] +     pix[2]
                   + 2.0*pix[3] + 4.0*pix[4] + 2.0*pix[5]
                   +     pix[6] + 2.0*pix[7] +     pix[8]  ) / 16.0;
  /**/

  /*/ Sharpen  
  gl_FragColor = ( -     pix[0] -     pix[1] -     pix[2]
                   -     pix[3] + 9.0*pix[4] -     pix[5]
                   -     pix[6] -     pix[7] -     pix[8]   );
  /**/
  /*/ Sharpen 2
  gl_FragColor = (              -     pix[1]             
                   -     pix[3] + 5.0*pix[4] -     pix[5]
                                -     pix[7]                );
  /**/
  /*/ Sharpen 3
  gl_FragColor = ( +     pix[0] - 2.0*pix[1] +     pix[2]
                   - 2.0*pix[3] + 5.0*pix[4] - 2.0*pix[5]
                   +     pix[6] - 2.0*pix[7] +     pix[8]   );
  /**/

  /*/ Laplacian  
  gl_FragColor = ( -     pix[0] -     pix[1] -     pix[2]
                   -     pix[3] + 8.0*pix[4] -     pix[5]
                   -     pix[6] -     pix[7] -     pix[8]   );
  /**/




  // Gaussian noise
  //gl_FragColor = texture2D(sampler0, gl_TexCoord[0].st) - 3.0*noise4(gl_TexCoord[0].st); 


  //gl_FragColor.rgb = vec3(1,0,0);
  //gl_FragColor = gl_Color;

}
