
/** Correct camera lens distortion of img_src and stores the result in img_dst.

    The input float array f_dist contains the coefficient of radial distortion, and f_proj contains the intrinsinc parameters of the camera: center of projection (x and y); focal length in pixels (x and y). The focal lengths are the same when the pixels are square.

    Reference:

    http://www.cognotics.com/opencv/docs/1.0/ref/opencvref_cv.htm#cv_3d

  */

// (IN_TEX: VglImage* img_src, OUT_FBO: VglImage* img_dst, float* f_dist, float* f_proj)

uniform sampler2D img_src;
uniform vec2 tex_size; // img_src->width, img_src->height
uniform vec4 dist; // f_dist[0], f_dist[1], f_dist[2], f_dist[3]
uniform vec4 proj; // f_proj[2],  f_proj[5], f_proj[0], f_proj[4]

vec2 lookup(vec2   coord)
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
   float r2 = r*r * 1.0;
   float r4 = r2*r2;
   float coeff = (k1*r2+k2*r4);
 
   coord = ((coord + coord*coeff.xx) * ff) + float2(ox,oy);

   coord.xy /= tex_size.xy;
   return coord;
}

void main(
    vec4 dist,
    vec4 proj )
{
 
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


  mat3 iden = mat3(1.0, 0.0, 0.0,
                   0.0, 1.0, 0.0,
                   0.0, 0.0, 1.0); 

  gl_FragColor  = texture2D(img_src, lookup(gl_TexCoord[0].st) );

}
