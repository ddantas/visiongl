
/** Do the same as vglFindDisparity, but the smallest difference is stored in img_best, and corresponding disparity in img_disp. Both are input and output images.

  */

// (IN_TEX: VglImage* img_sum, IN_OUT: VglImage* img_disp, IN_OUT: VglImage* img_best, float disparity)

uniform sampler2D img_sum;
uniform sampler2D img_disp;
uniform sampler2D img_best;
uniform vec2 tex_size; // img_sum->ipl->width, img_sum->ipl->height
uniform float disparity; // disparity

void main(void){
  vec4  color_sum;
  float gray_best;
  float gray_disp;

  color_sum        = texture2D(img_sum,  gl_TexCoord[0].xy);
  gray_best        = texture2D(img_best, gl_TexCoord[0].xy);
  gl_FragData[0].r = texture2D(img_disp, gl_TexCoord[0].xy);
  gl_FragData[1].r = gray_best;

  if (color_sum.r < gray_best.r){
    gl_FragData[1].r = color_sum.r;         //gray_best.r = ...
    gl_FragData[0].r = (disparity) / 256.0; //gray_disp.r = ...
  }
  if (color_sum.g < gray_best.r){
    gl_FragData[1].r = color_sum.g;
    gl_FragData[0].r = (disparity + 1.0) / 256.0;
  }
  if (color_sum.b < gray_best.r){
    gl_FragData[1].r = color_sum.b;
    gl_FragData[0].r = (disparity + 1.0) / 256.0;
  }
  if (color_sum.a < gray_best.r){
    gl_FragData[1].r = color_sum.a;
    gl_FragData[0].r = (disparity + 1.0) / 256.0;
  }

  //gl_FragData[0].r = .7; //gray_best = .7;
  //gl_FragData[1].r = .3; //gray_disp = .3;


}
