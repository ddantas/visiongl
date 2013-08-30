
/** Converts image RGB to BGR color space

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){
        vec3 pix = texture2D(sampler0, gl_TexCoord[0].xy);
        float r = pix.r;
        float g = pix.g;
        float b = pix.b;

        gl_FragColor.bgr = vec3(r, g, b*1.0);

}
