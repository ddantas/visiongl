
/** Return one step of thinning. 
    Algorithm by Bernard and Manzanera 1999.
    Receive as input the image to be thinned and its erosion by a elementary 
cross structuring element.
    Neighborhood pixels are indexed as follows:

\f$
\begin{array}{ccc}

    P3  &  P2  &  P1    \\
    P4  &  P8  &  P0    \\
    P5  &  P6  &  P7

\end{array}
\f$

    References:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    T. M. Bernard and A. Manzanera, Improved low complexity fully parallel
        thinning algorithms, 1999

  */

// (IN_TEX: VglImage* src, IN_TEX: VglImage* eroded, OUT_FBO: VglImage* dst)

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
                  vec2( 0.0,  0.0),
                };

void main(void){
  vec4 p_src[9];
  vec4 p_ero[9];
  int i;

  p_src[8] = texture2D(src,    gl_TexCoord[0].xy);
  p_ero[8] = texture2D(eroded, gl_TexCoord[0].xy);

  if (  (p_src[8].r != 0.0) && (p_ero[8].r == 0.0)  ){

    for (i = 0; i < 8; i++){
      p_src[i] = texture2D(src, 
                             gl_TexCoord[0].xy + offset[i] / tex_size);
      p_ero[i] = texture2D(eroded, 
                             gl_TexCoord[0].xy + offset[i] / tex_size);
    }

    if (    ( (p_src[4].r == 0.0) && (p_ero[0].r != 0.0) ) ||
            ( (p_src[6].r == 0.0) && (p_ero[2].r != 0.0) ) ||
            ( (p_src[0].r == 0.0) && (p_ero[4].r != 0.0) ) ||
            ( (p_src[2].r == 0.0) && (p_ero[6].r != 0.0) ) ||

            ( (p_src[0].r == 0.0) && (p_src[1].r == 0.0) && (p_src[2].r == 0.0) && (p_ero[5].r != 0.0) ) ||
            ( (p_src[2].r == 0.0) && (p_src[3].r == 0.0) && (p_src[4].r == 0.0) && (p_ero[7].r != 0.0) ) ||
            ( (p_src[4].r == 0.0) && (p_src[5].r == 0.0) && (p_src[6].r == 0.0) && (p_ero[1].r != 0.0) ) ||
            ( (p_src[6].r == 0.0) && (p_src[7].r == 0.0) && (p_src[0].r == 0.0) && (p_ero[3].r != 0.0) )     ){

      if (    ( (p_src[0].r == 0.0) && (p_src[1].r != 0.0) && (p_src[2].r == 0.0) ) ||
              ( (p_src[2].r == 0.0) && (p_src[3].r != 0.0) && (p_src[4].r == 0.0) ) ||
              ( (p_src[4].r == 0.0) && (p_src[5].r != 0.0) && (p_src[6].r == 0.0) ) ||
              ( (p_src[6].r == 0.0) && (p_src[7].r != 0.0) && (p_src[0].r == 0.0) )     ){

        gl_FragColor = p_src[8];
      }
      else{
        gl_FragColor = 0.0;
      }
    }
    else{
      gl_FragColor = p_src[8];
    }
    //gl_FragColor = 1.0;

  }
  else{
    gl_FragColor = p_src[8];
    //gl_FragColor = 0.0;
  }



}
