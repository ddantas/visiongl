
/** Apply homography in img_src and stores result in img_dst.

    Important: for matrices the cmponents are written in 
    column major order: 

    \c mat2 \c m = \c mat2 (1, 2, 3, 4)
\f$
    \Leftrightarrow
\f$
    \c m = 
\f$
    \left( 
      \begin{array}{cc}
          1  &  3        \\

          2  &  4
      \end{array}
    \right) 

\f$

    In C we build the matrix in line major order, then we must transpose
    tbe matrix before using it in OpenGL context.

  */

// (IN_TEX: VglImage* img_src, OUT_FBO: VglImage* img_dst, float* f_homo)

uniform sampler2D img_src;
uniform vec2 tex_size; // img_src->width, img_src->height
uniform mat3 homo; // f_homo


// dst(x?,y?)<-src(x,y)
// (x?,y?)^T^=map_matrix•(x,y,1)^T^+b if CV_WARP_INVERSE_MAP is not set,
// (x, y)^T^=map_matrix•(x?,y',1)^T^+b otherwise

vec2 rectify(  mat3 H, 
               vec2 coords )
{
  vec3 homogCoord;
  coords.xy *= tex_size.xy;
  
  homogCoord.xy = coords.xy; 
  homogCoord.z  = 1.0;
  vec3 rectifiedCoord = transpose(H) * homogCoord;
  rectifiedCoord.xy = rectifiedCoord.xy / rectifiedCoord.zz;
  //rectifiedCoord.xy = rectifiedCoord.xy / .78;
  //rectifiedCoord.x /= rectifiedCoord.z;
  /*
  rectifiedCoord.x /= tex_size.x;
  rectifiedCoord.y /= tex_size.y;
  rectifiedCoord.z /= 1.0;
  */

  rectifiedCoord.xy /= tex_size.xy;
  return rectifiedCoord.xy;
}


void main(
    vec4 dist,
    vec4 proj/*,
    mat3 homo*/ )
{
  //tex_size = (640, 480);
  //tex_size = (1, 1);
  
  mat3 homo2 = mat3(
		    0.780755,
		    0.000000,
		    0.000000,
		    -0.126815,
		    0.780755,
		    81.161835,
		    -0.000343,
		    -0.000000,
		    1.000000);
  mat3 homo3 = mat3(
		    1.056853,
		    0.000000,
		    0.000000,
		    -0.471965,
		    0.370713,
		    302.057831,
		    0.000089,
		    0.000000,
		    1.000000);

  mat3 homo4 = mat3(1.131633, 0.000000,  -0.000000,
		    0.002121, 1.168525, -19.065924,
		    0.000206, 0.000000,   1.000000);

  mat3 inv4  = mat3(  0.88368,   -0.00000,   -0.00000,
                     -0.00457,    0.85578,   16.31569,
	             -0.00018,    0.00000,    1.00000);

  mat3 iden = mat3(1.0, 0.0, 0.0,
                   0.0, 1.0, 0.0,
                   0.0, 0.0, 1.0); 

  vec2 coord = rectify(homo, gl_TexCoord[0].st);
  //gl_FragColor.rgb  = texture2D(img_src, coord.xy );
  //gl_FragColor.rgb  = coord.rgb;
  //vec2 coord = gl_TexCoord[0].st;
  //gl_FragColor  = texture2D(img_src, lookup(coord) );
  gl_FragColor  = texture2D(img_src, coord );
  //gl_FragColor.rg  = lookup(coord);

}
