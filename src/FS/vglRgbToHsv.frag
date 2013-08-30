
/** Converts image RGB to HSV color space

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){
        vec3 pix = texture2D(sampler0, gl_TexCoord[0].xy);
        float r = pix.r;
        float g = pix.g;
        float b = pix.b;
	float the_max, the_min, delta;
	float h, s, v;

	the_min = min(min(r, g), b);
	the_max = max(max(r, g), b);
	v = the_max;				// v

	delta = the_max - the_min;

	if( the_max != 0.0 )
		s = delta / the_max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		s = 0.0;
		h = -1.0;
		return;
	}

	if( r == the_max )
		h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == the_max )
		h = 2.0 + ( b - r ) / delta;	// between cyan & yellow
	else
		h = 4.0 + ( r - g ) / delta;	// between magenta & cyan

	if( h < 0.0 )                           // h in [-1, 5]
		h += 6.0;                       // h in [0, 6]
                    
	h /= 6.0;				// h in [0, 1]

        gl_FragColor.bgr = vec3(h, s, v);

}
