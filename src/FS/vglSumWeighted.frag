
/** Weighted sum of two images. The first image is multiplied by weight, and the second, by 1 - weight. Default weight is 0.5.

  */

// (IN_TEX: VglImage* src0, IN_TEX: VglImage* src1, OUT_FBO: VglImage* dst, float weight = .5)

uniform sampler2D sampler0;
uniform sampler2D sampler1;

uniform float weight; //weight

void main(void){

  gl_FragColor = (        weight  * texture2D(sampler0, gl_TexCoord[0].st) +
                   (1.0 - weight) * texture2D(sampler1, gl_TexCoord[0].st)   ); 

}
