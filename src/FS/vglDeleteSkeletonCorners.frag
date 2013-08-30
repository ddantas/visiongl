
/** Deletes corner from skeleton.

    Receive as input the image with the skeleton to be thinned. Receives 
    also the step. must be called once with step 1 and once with step 2.

    Neighborhood pixels is indexed as follows:

\f$
\begin{array}{ccc}

    P3  &  P2  &  P1      \\
    P4  &  P8  &  P0      \\
    P5  &  P6  &  P7

\end{array}
\f$


    Pixels deleted are the ones that mach the pattern and its rotations 
    by 90deg.

\f$
\begin{array}{ccc}

    0  &  0  &  x      \\
    0  &  1  &  1      \\
    x  &  1  &  0

\end{array}
\f$

    References:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    T. M. Bernard and A. Manzanera, Improved low complexity fully parallel
        thinning algorithms, 1999

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, int step)

uniform sampler2D src;
uniform int step; // step

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
  int i;

  p_src[8] = texture2D(src,    gl_TexCoord[0].xy);

  if (p_src[8].r != 0.0){

    for (i = 0; i < 8; i++){
      p_src[i] = texture2D(src, 
                             gl_TexCoord[0].xy + offset[i] / tex_size);
    }

    if (step == 1){

      if (    (  (p_src[0].r != 0.0) && (p_src[6].r != 0.0) &&       //SE
                 (p_src[2].r == 0.0) && (p_src[3].r == 0.0) &&
                 (p_src[4].r == 0.0) && (p_src[7].r == 0.0)    )  || 
              (  (p_src[4].r != 0.0) && (p_src[6].r != 0.0) &&       //SW
                 (p_src[0].r == 0.0) && (p_src[1].r == 0.0) &&
                 (p_src[2].r == 0.0) && (p_src[5].r == 0.0)    )           ){
        gl_FragColor = 0.0;
        //gl_FragColor.r = 1.0;
      }
      else{
        gl_FragColor = p_src[8];
      }

    }
    else{

      if (    (  (p_src[0].r != 0.0) && (p_src[2].r != 0.0) &&       //NE
                 (p_src[1].r == 0.0) && (p_src[4].r == 0.0) &&
                 (p_src[5].r == 0.0) && (p_src[6].r == 0.0)    )  || 
              (  (p_src[2].r != 0.0) && (p_src[4].r != 0.0) &&       //NW
                 (p_src[0].r == 0.0) && (p_src[3].r == 0.0) &&
                 (p_src[6].r == 0.0) && (p_src[7].r == 0.0)    )           ){
        gl_FragColor = 0.0;
        //gl_FragColor.g = 1.0;
      }
      else{
        gl_FragColor = p_src[8];
      }

    }
  }
  else{
    gl_FragColor = p_src[8];
  }
  //gl_FragColor.r = 1;
}
