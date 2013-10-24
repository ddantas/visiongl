
/** Calculate Mandelbrot set

  */

// (OUT_FBO: VglImage* dst, float ox = 0.0, float oy = 0.0, float half_win = 1.0)

uniform float ox; // ox
uniform float oy; // oy
uniform float half_win; // half_win

void main(void){
  const float MAX_ITER = 200.0;
  const float LIM = 2.0;
  const float EPS = .0000001;
  float i;

  float real;
  float imag;
  float c_real;
  float c_imag;
  float buf_real;
  float buf_imag;
  float r2;

  real = gl_TexCoord[0].x * 2.0 * half_win + ox - half_win;
  imag = gl_TexCoord[0].y * 2.0 * half_win + oy - half_win;
  c_real = real;
  c_imag = imag;

  for (i = 0.0; i < MAX_ITER; i++){
    //printf("iter[%d]: z = (%f, %f)\n", i, real, imag);
    buf_real = real;
    buf_imag = imag;
    real = c_real + (buf_real * buf_real) - (buf_imag * buf_imag);
    imag = c_imag + 2.0 * (buf_real * buf_imag);
    //r2 = (real * real) + (imag * imag);
    if (  (real > LIM) || (real < -LIM) || (imag > LIM) || (imag < -LIM)  ) 
    {
      break;
    } 
    /*
    if (  (abs(real) > LIM) && 
          (abs(imag) > LIM)     ){
      break;
    }
    */
    /*
    if (  (abs(real - buf_real) < EPS) && 
          (abs(imag - buf_imag) < EPS)     ){
      i = MAX_ITER;
      break;
    }
    */
  }

  gl_FragColor.r = 1.0 - (i/MAX_ITER);
  gl_FragColor.g = fract(i/20.0);
  if (i >= MAX_ITER)
  {
    gl_FragColor.r = 0.0;
    gl_FragColor.g = 0.0;
  }
  gl_FragColor.b = 0.0;

}
