
/** Return one step of thinning.
    Algorithm by Chin, Wan Stover and Iverson, 1987.
    Receive as input the image to be thinned, buffer image and number 
    of times to iterate.
    Neighborhood pixels are indexed as follows:

\f$ 
\begin{array}{ccccc}

    x   & x   & P10 & x   & x    \\

    x   & P3  & P2  & P1  & x    \\

    P11 & P4  & P0  & P8  & P9   \\

    x   & P5  & P6  & P7  & x    \\

    x   & x   & P12 & x   & x

\end{array}
\f$ 

    References:

    M. Couprie, Note on fifteen 2D parallel thinning algorithms, 2006

    R. T. Chin et al., A one-pass thinning algorithm and its parallel 
        implementation, 1987
  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D src;

uniform vec2 tex_size; // src->ipl->width, src->ipl->height

vec2 offset[] = { vec2( 0.0,  0.0), //P0

                  vec2( 1.0, -1.0), //P1
                  vec2( 0.0, -1.0),
                  vec2(-1.0, -1.0),
                  vec2(-1.0,  0.0),
                  vec2(-1.0,  1.0),
                  vec2( 0.0,  1.0),
                  vec2( 1.0,  1.0),
                  vec2( 1.0,  0.0), //P8

                  vec2( 2.0,  0.0), //p9
                  vec2( 0.0, -2.0), //p10
                  vec2(-2.0,  0.0), //p11
                  vec2( 0.0,  2.0), //p12
                };

void main(void){
  vec4 pixel0;
  float pixel;
  bool p[13];
  int i;

  pixel0 = texture2D(src,    gl_TexCoord[0].xy);

  if (pixel0.r != 0.0){

    for (i = 0; i < 13; i++){
      pixel = texture2D(src, 
                             gl_TexCoord[0].xy + offset[i] / tex_size).r;
      if (pixel == 0.0){
        p[i] = false;
      }
      else{
        p[i] = true;
      }
    }

    if (    ( (p[4].r) && (p[6].r) && (p[8].r) && (!p[1].r) && (!p[2].r) && (!p[3].r) )   ||  //(1)
            ( (p[6].r) && (p[8].r) && (p[2].r) && (!p[3].r) && (!p[4].r) && (!p[5].r) )   ||
            ( (p[8].r) && (p[2].r) && (p[4].r) && (!p[5].r) && (!p[6].r) && (!p[7].r) )   ||
            ( (p[2].r) && (p[4].r) && (p[6].r) && (!p[7].r) && (!p[8].r) && (!p[1].r) )   ||
         
            ( (p[4].r) && (p[6].r) && (!p[8].r) && (!p[1].r) && (!p[2].r) )   ||              //(2)
            ( (p[6].r) && (p[8].r) && (!p[2].r) && (!p[3].r) && (!p[4].r) )   ||
            ( (p[8].r) && (p[2].r) && (!p[4].r) && (!p[5].r) && (!p[6].r) )   ||
            ( (p[2].r) && (p[4].r) && (!p[6].r) && (!p[7].r) && (!p[8].r) )                  )
    {
      if (    ( (!p[4].r) && (p[8].r) && (!p[9].r) )    ||
              ( (!p[2].r) && (p[6].r) && (!p[12].r) )       ) 
      {
        gl_FragColor = pixel0;
      }
      else{
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
      }

    }
    else{
      gl_FragColor = pixel0;
    }

  }
  else{
    gl_FragColor = pixel0;
  }

}
