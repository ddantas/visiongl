
/** PassThrough vertex shader
  */

void main(void){

  vec4 clipCoord = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_Position = clipCoord;

  /*
  gl_FrontColor = gl_Color;

  vec3 ndc = clipCoord.xyz / clipCoord.w;

  gl_SecondaryColor.rgb = ndc;
  */
 
  //gl_Position = gl_Vertex;
  //gl_FrontColor = gl_Color;
  //gl_FrontColor = 1;

  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;


}
