
/** Feature Points are defined as function of the crossing number
and number of neighbors of a pixel. 

The number of neighbors is indicated as Nb. Crossing number is defined as

Nc = number of occurrences of the pattern 01 in the neighborhood of P

    Neighborhood pixels are indexed as follows:

    P3 P2 P1

    P4 P  P0

    P5 P6 P7


    All the ending points are feature points. Are defined as
Se = { P | Nc(P) = 1 }


    Feature points type 1, denoted as S1, are defined as
S1 = { P | Nc(P) >= 3}

    Feature points type 2, denoted as S2, are defined as
S1 = { P | Nb(P) >= 3}

    Feature points type 3, denoted as S3, are defined as
S3 = { P | Nc(P) >= 3 or Nb(P) >= 4 }


    References:

    Ke Liu et al., Identification of fork points on the skeletons
        of handwritten chinese characters

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, int type)

uniform sampler2D src;

uniform int  type; // type
uniform vec2 tex_size; // src->ipl->width, src->ipl->height

vec2 offset[] = { vec2( 0.0,  1.0),
                  vec2( 1.0,  1.0),
                  vec2( 1.0,  0.0),
                  vec2( 1.0, -1.0),
                  vec2( 0.0, -1.0),
                  vec2(-1.0, -1.0),
                  vec2(-1.0,  0.0),
                  vec2(-1.0,  1.0),
                };

void main(void){
  vec4 p0;
  vec4 p_src[9];
  int i;
  float c = 0.0;
  float b = 0.0;
  float delta = 1.0 / 8.0;

  p0 = texture2D(src, gl_TexCoord[0].xy);

  if (p0.r != 0.0) {

    for (i = 0; i < 8; i++){
      p_src[i] = texture2D(src, 
                             gl_TexCoord[0].xy + offset[i] / tex_size);
    }
    p_src[8] = p_src[0];

    for (i = 0; i < 8; i++){
      if ( (p_src[i].r == 0.0) && (p_src[i+1].r != 0.0) ){
        c = c + delta;
      } 
      if ( (p_src[i].r != 0.0) ){
        b = b + delta;
      }
    }

    if ( (c == 1.0/8.0)                                  ||
         (type == 1 &&   c >= 3.0/8.0)                   ||
         (type == 2 &&   b >= 3.0/8.0)                   ||
         (type == 3 && ( c >= 3.0/8.0 || b >= 4.0/8.0))     ){
      gl_FragColor = 1.0;
    }
    else{
      gl_FragColor = 0.0;
    }
  }
  else{
    gl_FragColor = 0.0;
  }

}
