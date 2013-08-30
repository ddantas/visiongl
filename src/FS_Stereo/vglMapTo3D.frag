/** Convert depth map to affine reconstruction

    This algorithm ignores the infinite homography.

    img_map: input depth map

    img_3d: output reconstruction

    f: focal length in pixels

    b: baseline in cm

    D: fixation point or maximum depth

    is_float: if true, output image will store z in cm. If false
              output image will store z as 255 * (depth / D). 
              if depth == D then z = 0.

    disp_k: If set, single disparity will be used.

    h: height of camera in cm
 */
// (IN_TEX: VglImage* img_map, OUT_FBO: VglImage* img_3d, float f, float b, float D, float disp_k = 0.0, float h = 10.0){

uniform sampler2D img_map;

uniform float f;       // f
uniform float b;       // b
uniform float D;       // D
uniform float is_float; // (img_3d->depth == IPL_DEPTH_32F ? 1.0 : 0.0)
uniform float disp_k;  // disp_k
uniform float h;       // h

uniform float max_u; //  1.0
uniform float min_u; // 0.0
uniform float max_v; // 1.0
uniform float min_v; // 0.0
uniform float s_x;   // 0.5
uniform float s_y;   //  390.0 / 480.0  /* thiagopx0, 1, ddantas0, 1*/
//uniform float s_y = 410.0 / 480.0;  /* rodrigo0, jb0 */

void main(){

  float x;
  float y;
  float u = gl_TexCoord[0].x;
  float v = gl_TexCoord[0].y;

  //for (int u = 1; u < max_u; u++){
    //for (int v = 1; v < max_v; v++){
      float value = texture2D(img_map, gl_TexCoord[0].xy).r;
      float disp_var = value;
      float result;
      float disp;
      if (disp_var == 0.0){
        result = 0.0;
      }
      else
      {
        if (disp_k == 0.0){
          disp = disp_var;
	}
	else {
          disp = disp_k;
	}
        float phi_k   = disp_k / f;
        //float phi     = disp   / f;
        float theta = b / D;
        //float d   = (disp * D) / ( f * (phi)   + (theta) );
        //float e   =         b  / (     (phi)   + (theta) );
        float e_k =          b  / (     (phi_k) + (theta) );
        float z   = e_k * (1 - phi_k*phi_k/2);
        float delta_v = (h/z - h/D) * f;
              x   = (u - s_x)           * e_k / D + s_x;
              y   = (v - s_y - delta_v) * e_k / D + s_y;
	result = D - round(z);


        gl_FragColor.r = disp_k;

      }

      if (is_float){
        gl_FragColor.r = result;
      }
      else
      {
        //gl_FragColor.r = phi_k; //1.0 * (result / D);
      }
    //}
  //}
}
