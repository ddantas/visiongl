
/** Image src0 minus src1.

  */

// (IN_TEX: VglImage* src0, IN_TEX: VglImage* src1, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;
uniform sampler2D sampler1;

void main(void){

  gl_FragColor = texture2D(sampler0, gl_TexCoord[0].st) -
                 texture2D(sampler1, gl_TexCoord[0].st)   ; 

}
