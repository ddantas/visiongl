
/** Deletes warts from skeleton. Receive as input the image with the skeleton to be thinned. Neighborhood pixels are indexed as follows:

    P3 P2 P1

    P4 P  P0/8

    P5 P6 P7

    Pixels deleted are the ones that mach the pattern and its rotations 
    by 45deg.

    1 0 0
    1 1 0
    1 0 0

    1 1 0
    1 1 0
    1 0 0

    1 1 0
    1 1 0
    1 1 0

    1 1 1
    1 1 0
    1 1 0

    1 1 1
    1 1 0
    1 1 1


    That is the same as delete the pixels with crossing number = 1 and 
neighbor number >=3

    References:

    Ke Liu et al., Identification of fork points on the skeletons
        of handwritten chinese characters

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D src;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

vec2 offset[] = { vec2( 0.0,  1.0),
                  vec2( 1.0,  1.0),
                  vec2( 1.0,  0.0),
                  vec2( 1.0, -1.0),
                  vec2( 0.0, -1.0),
                  vec2(-1.0, -1.0),
                  vec2(-1.0,  0.0),
                  vec2(-1.0,  1.0),
                  vec2( 0.0,  0.0),
                };

void main(void){
  vec4 pixel0;
  float pixel[9];
  int i;
  float c = 0.0;
  float b = 0.0;
  float delta = 1.0 / 8.0;

  pixel0 = texture2D(src,    gl_TexCoord[0].xy);

  if (pixel0.r != 0.0){

    for (i = 0; i < 8; i++){
      pixel[i] = texture2D(src, 
                             gl_TexCoord[0].xy + offset[i] / tex_size).r;
    }
    pixel[8] = pixel[0];

    for (i = 0; i < 8; i++){
      if ( (pixel[i] == 0.0) && (pixel[i+1] != 0.0) ){
        c = c + delta;
      } 
      if ( (pixel[i] != 0.0) ){
        b = b + delta;
      }
    }

    if ( (c == 1.0/8.0) && (b >= 3.0/8.0) ){
      gl_FragColor = 0.0;
    }
    else{
      gl_FragColor = pixel0;
    }
    /*
    if (c == 1.0/8.0){
      gl_FragColor.r = 1.0;
    }
    else{
      gl_FragColor.r = 0.0;
    }

    if (b >= 3.0/8.0){
      gl_FragColor.g = 1.0;
    }
    else{
      gl_FragColor.g = 0.0;
    }

    gl_FragColor.b = 1.0;
    */


  }
  else{
    gl_FragColor = pixel0;
  }



}
