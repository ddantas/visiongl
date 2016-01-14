
/** Undistort, correct projection and rectify img_src and stores result in img_dst, for use with stereo algorithm

    The input float array f_dist contains the coefficient of radial distortion, and f_proj contains the intrinsinc parameters of the camera: center of projection (x and y); focal length in pixels (x and y). The focal lengths are the same when the pixels are square.

    The input float array f_homo contains the homography that rectifies the image.

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

// (IN_TEX: VglImage* img_src, OUT_FBO: VglImage* img_dst, float* f_dist, float* f_proj, float* f_homo)

uniform sampler2D img_src;
uniform vec2 tex_size; // img_src->getWidth(), img_src->getHeight()
uniform vec4 dist; // f_dist[0], f_dist[1], f_dist[2], f_dist[3]
uniform vec4 proj; // f_proj[2],  f_proj[5], f_proj[0], f_proj[4]
uniform mat3 homo; // f_homo

//uniform vec4 proj; // 2.0*f_proj[0]/img_src->width-1.0,  2.0*f_proj[4]/img_src->height-1.0, f_proj[2]/img_src->width, f_proj[5]/img_src->height

vec2 lookup(  vec2   coord/*,
                vec4   dist, //k1, k2, k3, k4
                vec4   proj*/) //ox, oy, f1, f2
{
   float k1 = dist.x;
   float k2 = dist.y;
   float k3 = dist.z;
   float k4 = dist.w;
   float ox = proj.x;
   float oy = proj.y;
   float f1 = proj.z;
   float f2 = proj.w;

   coord.xy *= tex_size.xy;

   vec2 ff = vec2(f1,f2);
   coord = (coord.xy - vec2(ox,oy)) / ff;
   float x1 = coord.x;
   float y1 = coord.y;
   
   float r = sqrt(  ((x1))*((x1))+((y1))*((y1)) );
   float r2 = r*r;
   float r4 = r2*r2;
   float coeff = (k1*r2+k2*r4);
 
   coord = ((coord + coord*coeff.xx) * ff) + float2(ox,oy);

   coord.xy /= tex_size.xy;
   return coord;
}

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
  tex_size = (640, 480);
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
  gl_FragColor  = texture2D(img_src, lookup(coord) );
  //gl_FragColor.rg  = lookup(coord);

}
