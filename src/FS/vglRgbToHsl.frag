
/** Converts image RGB to HSL color space

  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void){
        vec3 pix = texture2D(sampler0, gl_TexCoord[0].xy);
        float r = pix.r;
        float g = pix.g;
        float b = pix.b;
	float the_max, the_min, delta;
	float h, s, l;

	the_min = min(min(r, g), b);
	the_max = max(max(r, g), b);

	delta = the_max - the_min;
	l = delta / 2.0;			// v

	if( l == 0.0 ){
		s = 0.0;			// s
	}
	else if (l <= .5) {
	  s = (the_max - the_min) / delta;
	}
	else{
	  s = (the_max - the_min) / (2.0 - delta);
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

        gl_FragColor.bgr = vec3(h, s, l);

}
