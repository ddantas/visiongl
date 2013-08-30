
/** Converts image RGB to XYZ color space.

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){
        vec3 pix = texture2D(sampler0, gl_TexCoord[0].xy);

        const mat3 RgbToCie = mat3(0.412453, 0.212671, 0.019334,
                                   0.357580, 0.715160, 0.119193,
                                   0.180423, 0.072169, 0.950227);

        gl_FragColor.rgb = pix.rgb * RgbToCie;

}
