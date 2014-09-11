
/** Inverts 3d image.

    As the wrappers are implemented currently, the shader will invert only the first layer of the 3d image.

  */
// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler3D sampler0;

void main(void){

  gl_FragColor = 1.0-texture3D(sampler0, gl_TexCoord[0].xy); 

}
