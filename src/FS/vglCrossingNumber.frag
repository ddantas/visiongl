
/** Crossing number is defined as the number of ocurrences of the pattern 01
in the neihborhood of a pixel.

    Neighborhood of pixel P is indexed as follows:

\f$
\begin{array}{ccc}

    P3   &  P2   &  P1      \\ 
    P4   &  P    &  P0/8    \\
    P5   &  P6   &  P7

\end{array}
\f$


    References:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    T. M. Bernard and A. Manzanera, Improved low complexity fully parallel
        thinning algorithms, 1999

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D src;
uniform sampler2D eroded;

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
  vec4 p;
  vec4 p_src[9];
  int i;
  float c = 0.0;
  float delta_c = 1.0 / 4.0;

  p = texture2D(src, gl_TexCoord[0].xy);

  if (p.r != 0.0) {

    for (i = 0; i < 8; i++){
      p_src[i] = texture2D(src, 
                             gl_TexCoord[0].xy + offset[i] / tex_size);
    }
    p_src[8] = p_src[0];

    for (i = 0; i < 8; i++){
      if ( (p_src[i].r == 0.0) && (p_src[i+1].r != 0.0) ){
        c = c + delta_c;
      } 
    }

    gl_FragColor = c;
  }
  else{
    gl_FragColor = 0.0;
  }



}
