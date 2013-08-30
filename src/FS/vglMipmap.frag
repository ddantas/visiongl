
/** Get specified level of detail.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst, float lod)

uniform sampler2D sampler0;
uniform float lod; // lod

void main(void){

  gl_FragColor = texture2DLod(sampler0, gl_TexCoord[0].xy, lod);

}
