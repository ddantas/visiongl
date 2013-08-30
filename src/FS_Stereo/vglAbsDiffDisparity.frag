
/** Calculate absolute difference between img_ref and img_2. Disparities considered are in the closed interval [4*disparity, 4*disparity+3].

    The four differences are stored in the RGBA image dst.

  */

// (IN_TEX: VglImage* img_ref, IN_TEX: VglImage* img_2, OUT_FBO: VglImage* dst, float disparity)

uniform sampler2D img_ref;
uniform sampler2D img_2;
uniform vec2 tex_size; // img_ref->ipl->width, img_ref->ipl->height
uniform float disparity; // disparity

//#define FRAG_COLOR (diff.r + diff.g + diff.b)
#define FRAG_COLOR dot(diff, diff)

void main(void){
  vec3 color_ref = texture2D(img_ref, gl_TexCoord[0].st);
  vec3 color_2;
  vec3 diff;
  vec2 disparity_2;
  
  disparity_2 = vec2(disparity / tex_size.s, 0.0);
  color_2   = texture2D(img_2, gl_TexCoord[0].st + disparity_2);
  diff      = abs (color_2 - color_ref);
  gl_FragColor.r = FRAG_COLOR;

  disparity_2 = vec2((disparity + 1.0) / tex_size.s, 0.0);
  color_2   = texture2D(img_2, gl_TexCoord[0].st + disparity_2);
  diff      = abs (color_2 - color_ref);
  gl_FragColor.g = FRAG_COLOR;

  disparity_2 = vec2((disparity + 2.0) / tex_size.s, 0.0);
  color_2   = texture2D(img_2, gl_TexCoord[0].st + disparity_2);
  diff      = abs (color_2 - color_ref);
  gl_FragColor.b = FRAG_COLOR;

  disparity_2 = vec2((disparity + 3.0) / tex_size.s, 0.0);
  color_2   = texture2D(img_2, gl_TexCoord[0].st + disparity_2);
  diff      = abs (color_2 - color_ref);
  gl_FragColor.a = FRAG_COLOR;

  //gl_FragColor.rgb = diff;

}
