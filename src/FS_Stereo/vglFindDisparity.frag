
/** Find best disparity. The first input image, img_dif, contains absolute differences between a pair of images at disparities [4*disparity, 4*disparity+3]. 

    The second input image contains the smallest differences found in channel R, and corresponding disparity value in channel A, Is also an output image, and is updated whenever a smaller difference is found.

  */

// (IN_TEX: VglImage* img_dif, IN_OUT: VglImage* img_disp, float disparity)

uniform sampler2D img_dif;
uniform sampler2D img_disp;
uniform vec2 tex_size; // img_dif->ipl->width, img_dif->ipl->height
uniform float disparity; // disparity

void main(void){
  vec4 color_dif;
  vec4 color_disp;

  color_dif  = texture2D(img_dif,  gl_TexCoord[0].xy);
  color_disp = texture2D(img_disp, gl_TexCoord[0].xy);


  if (color_dif.r < color_disp.a){
    color_disp.a   = color_dif.r;
    color_disp.r = (disparity) / 256.0;
  }
  if (color_dif.g < color_disp.a){
    color_disp.a   = color_dif.g;
    color_disp.r = (disparity + 1.0) / 256.0;
  }
  if (color_dif.b < color_disp.a){
    color_disp.a   = color_dif.b;
    color_disp.r = (disparity + 2.0) / 256.0;
  }
  if (color_dif.a < color_disp.a){
    color_disp.a   = color_dif.a;
    color_disp.r = (disparity + 3.0) / 256.0;
  }

  gl_FragColor.rgb = color_disp.r;

}
