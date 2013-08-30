
/** VglSumDiffMipmap

    Sum of differences

  */

// (IN_TEX: VglImage* img_dif, OUT_FBO: VglImage* img_out, float max_lod)

uniform sampler2D img_dif;
uniform float max_lod;


void main(void){

  gl_FragColor = 0.0;

  max_lod = 4.0;
  for (float lod = 0.0; lod <= max_lod; lod++){
  //for (float lod = 4.0; lod <= 4.0; lod++){
    gl_FragColor += texture2DLod(img_dif,
				 gl_TexCoord[0].xy, lod);
  }

  gl_FragColor /= (max_lod + 1.0);
  //gl_FragColor = 1.0;

}
