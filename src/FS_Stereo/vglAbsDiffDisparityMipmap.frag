
/** Calculates average absolute difference between img_ref and img_2 at levels of detail in [0, max_lod]. Disparities considered are in the closed interval [4*disparity, 4*disparity+3].

    The four differences are stored in the RGBA image dst.

  */

// (IN_TEX: VglImage* img_ref, IN_TEX: VglImage* img_2, OUT_FBO: VglImage* dst, float disparity, float max_lod)

uniform sampler2D img_ref;
uniform sampler2D img_2;
uniform vec2 tex_size; // img_ref->ipl->width, img_ref->ipl->height
uniform float disparity; // disparity
uniform float max_lod; // max_lod

void main(void){
  vec3 color_ref;
  vec3 color_2;
  vec3 diff;
  vec2 disparity_2;

  gl_FragColor = 0.0;

  max_lod = 2.0;

  //for (float lod = 0.0; lod <= max_lod; lod++){
for (float lod = 0.0; lod <= 0.0; lod++){
  color_ref = texture2DLod(img_ref, gl_TexCoord[0].st, lod);

  disparity_2 = vec2(disparity / tex_size.s, 0.0);
  color_2   = texture2DLod(img_2, gl_TexCoord[0].st + disparity_2, lod);
  diff      = abs (color_2 - color_ref);
  gl_FragColor.r += (diff.r + diff.g + diff.b);

  disparity_2 = vec2((disparity + 1.0) / tex_size.s, 0.0);
  color_2   = texture2DLod(img_2, gl_TexCoord[0].st + disparity_2, lod);
  diff      = abs (color_2 - color_ref);
  gl_FragColor.g += (diff.r + diff.g + diff.b);

  disparity_2 = vec2((disparity + 2.0) / tex_size.s, 0.0);
  color_2   = texture2DLod(img_2, gl_TexCoord[0].st + disparity_2, lod);
  diff      = abs (color_2 - color_ref);
  gl_FragColor.b += (diff.r + diff.g + diff.b);

  disparity_2 = vec2((disparity + 3.0) / tex_size.s, 0.0);
  color_2   = texture2DLod(img_2, gl_TexCoord[0].st + disparity_2, lod);
  diff      = abs (color_2 - color_ref);
  gl_FragColor.a += (diff.r + diff.g + diff.b);
}

 gl_FragColor /= (3.0 * (max_lod+1));

  //  gl_FragColor.rgb = diff;



}
