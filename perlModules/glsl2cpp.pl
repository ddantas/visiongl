#!/usr/bin/perl -w

use File::Basename;
use perlModules::common qw( LineStartMultiLineComment
               LineStartSingleLineComment
               LineStartCleanComments
               LineStartParenthesis
               LineStartTypeStar
               LineStartVariable
               LineStartDefault
               LineStartSeparator
             );

#############################################################################
# LineStartCppValue
#
# Returns the string from the "//" to the first "\n" if found in 
# start of $line, 
# 
sub LineStartCppValue { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*//(.*)##;
  return ($1, $line);
}


#############################################################################
# LineStartHeader
#
# Returns the string after the "//" to the first "\n" if found 
# in start of $line, blank string if not found.
# 
sub LineStartHeader { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*//\s*(.*)\s*\n##;
  return ($1, $line);
}

#############################################################################
# LineStartSemicolon
#
# Returns the string after the ";" 
# in start of $line, blank string if not found.
# 
sub LineStartSemicolon { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*;(.*)##;
  return $1;
}

#############################################################################
# LineStartSemantics
#
# Returns the string after the semantic binding and the ":"
# in start of $line, blank string if not found.
# Valid semantic bindincs are IN_TEX, OUT_FBO and IN_OUT
# 
sub LineStartSemantics { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([a-zA-Z_]\w*)\s*:##;
  $semantics = $1;
  if ($semantics){
    if (!($semantics =~ m#^(IN_TEX|OUT_FBO|IN_OUT)$#))
    {
      die "Semantic $semantics invalid. Valid semantics are (IN_TEX|OUT_FBO|IN_OUT)\n";
    }
  }
  else{
    $semantics = "";
  }
  return ($semantics, $line);
}

#############################################################################
# LineStartUniform
#
# Returns the string after the first "uniform"  found
# in start of $line, blank string if not found.
# 
sub LineStartUniform { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*uniform(.*)\n##;
  return ($1, $line);
}

#############################################################################
# LineStartMain
#
# Returns the string that contains the main program, that is,
# everything after void main(void) including it.
# 
sub LineStartMain { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*((void)\s*(main)\s*\((\s*(void)\s*)\).*)\n##;
  return ($1, $line);
}

#############################################################################
# LineNonEmpty
#
# Returns the string from the first non space character, and "" if line is blank
# 
sub LineNonEmpty { # ($line) {
  my $line = $_[0];

  if ($line =~ m#^\s*(\S.*)$#){
      return ($1);
  }
  return "";
}

#############################################################################
# ProcessGlslUniform
#
# Receives as input a line and breaks it. Are expected a datatype, a 
# variable name, a semicolon and a single line comment.
#
#
sub ProcessGlslUniform { # ($line, $print) {
  my $line          = $_[0];
  my $print         = $_[1];

  my $type = "";
  my $type_gluniform = "";
  my $variable = "";
  my $cpp_value = "";
  my $aux = "";

    if ($print){
      print "Processing uniform line: $line\n";
    }

    ($type, $line) = LineStartTypeStar($line);
    if (!$type){
      print "Start-of-line type not found\n";
    }
    else{
      #print "After eliminating type:\n$line\n";
    }

    if ($type =~ m#^(mat)#){
      $type_gluniform .= "Matrix";
    }

    if ($type =~ m#([234])#){
      $type_gluniform .= "$1";
    }
    else{
      $type_gluniform .= "1";
    }

    if ($type =~ m#^(float|vec|mat)#){
      $type_gluniform .= "f";
    }
    elsif ($type =~ m#^(int|ivec)#){
      $type_gluniform .= "i";
    }

    if ($type =~ m#^(mat)#){
      $type_gluniform .= "v";
    }


    ($variable, $line) = LineStartVariable($line);
    if (!$variable){
      print "Start-of-line variable not found\n";
    }
    else{
      #print "After eliminating variable:\n$line\n";
    }

    $line = LineStartSemicolon($line);
    #print "no semicolon = $line\n";
    if (LineNonEmpty($line)){
      ($cpp_value, $line) = LineStartCppValue($line);
      #print "cpp value = $cpp_value\n";
    }

  if ($print){
    print "Type:      $type -> glUniform$type_gluniform\n";
    print "Variable:  $variable\n";
    print "CPP value: $cpp_value\n";
  }

  return ($type, $type_gluniform, $variable, $cpp_value);

}

#############################################################################
# ProcessGlslHeader
#
# Receives as input a GLSL header line and breaks it
#
#
sub ProcessGlslHeader { # ($line) {
  my $line          = $_[0];


  $line = LineStartParenthesis($line);
  if (!$line){
    die "Error: Start-of-line parenthesis not found\n";
  }
  else{
    #print "After eliminating parenthesis:\n$line\n";
  }  

  ($separator, $line) = LineStartSeparator($line);

  if ($separator and $separator eq ")"){
    print "No parameters found\n";
    return;
  }

  my $i = 0;

  $separator = ",";
  while ($separator and $separator eq ","){

    ($semantics[$i], $line) = LineStartSemantics($line);
    if (!$semantics[$i]){
      #print "Start-of-line semantics not found\n";
    }
    else{
      #print "After eliminating semantics:\n$line\n";
    }

    ($type[$i], $line) = LineStartTypeStar($line);
    if (!$type[$i]){
      print "Start-of-line type not found\n";
    }
    else{
      #print "After eliminating type:\n$line\n";
    }

    ($variable[$i], $line) = LineStartVariable($line);
    if (!$variable[$i]){
      print "Start-of-line variable not found\n";
    }
    else{
      #print "After eliminating variable:\n$line\n";
    }

    ($default[$i], $line) = LineStartDefault($line);
    if (!$default[$i]){
      #print "Start-of-line default not found\n";
    }
    else{
      #print "After eliminating variable:\n$line\n";
    }

    ($separator, $line) = LineStartSeparator($line);

    if ($separator and $separator eq ","){
      #print "Variable list continues\n";
    }

    print "Semantics[$i]: " . ($semantics[$i] or "") . "\n";
    print "Type     [$i]: $type[$i]\n";
    print "Variable [$i]: $variable[$i]\n";
    print "Default  [$i]: " . ($default[$i] or "") . "\n";

    $i++;
  }
  return ($semantics, $type, $variable);

}

#############################################################################
# ProcessGlslFile
#
# Receives as input a GLSL filename and generates CPP wrapper function
#
#
sub ProcessGlslFile { # ($filename, $output, $cpp_read_path) {
  my $filename      = $_[0];
  my $output        = $_[1];
  my $cpp_read_path = $_[2];
  
  my $comment; 
  my $semantics; 
  my $type;
  my $variable;
  my $default;
  my $uniform;

  undef $comment; 
  undef $semantics; 
  undef $type;
  undef $variable;
  undef $uniform;

  open GLSL, $filename;
  @list = <GLSL>;
  $line = join("", @list);
  close GLSL;  

  ($comment, $line) = LineStartMultiLineComment($line);
  if (!$comment){
    print "Start-of-file comment not found\n";
  }
  else{
    print "Writing this comment:\n$comment\n";
  }  

  ($perl_header, $line) = LineStartHeader($line);
  if (!$perl_header){
    print "Error: start-of-file header not found\n";
    return;
  }
  else{
    print "Writing this header:\n$perl_header\n";
  }

  ($semantics, $type, $variable, $default) = ProcessGlslHeader($perl_header);

  $line = LineStartCleanComments($line);

  my $i = 0;
  do {
    $line = LineStartCleanComments($line);
    ($uniform[$i], $line) = LineStartUniform($line);
    if ($uniform[$i]){
      print ("Uniform found: $uniform[$i]\n");
    }
    else{
      #print ("Uniform not found\n");
    }
    $i++;
  }
  while ($uniform[$i-1]);

  return  ($comment, $semantics, $type, $variable, $uniform);
}

#############################################################################
# PrintCppFile
#
# Receives as input a GLSL filename and generates CPP wrapper function
#
#
sub PrintCppFile { # ($basename, $comment, $semantics, $type, $variable, $default, $uniform, $output, $cpp_read_path) {
  my $basename      = $_[0];
  my $comment       = $_[1];
  my $semantics     = $_[2];
  my $type          = $_[3];
  my $variable      = $_[4];
  my $default       = $_[5];
  my $uniform       = $_[6];
  my $output        = $_[7];
  my $cpp_read_path = $_[8];

  my $i;
  my $first_framebuffer = "";

  print "Will write to $output.cpp and $output.h\n";

  open CPP, ">>", "$output.cpp";
  open HEAD, ">>", "$output.h";


  print CPP "$comment\n";
  print HEAD "$comment\n";

  print CPP "void $basename(";
  print HEAD "void $basename(";
  for ($i = 0; $i <= $#type; $i++){
    print CPP "$type[$i] $variable[$i]";
    print HEAD "$type[$i] $variable[$i]" . ($default[$i] or "");
    if ($i < $#type){
      print CPP ", ";
      print HEAD ", ";
    } 
  }
  print CPP "){\n";
  print HEAD ");\n\n";

  for ($i = 0; $i <= $#type; $i++){
    if ($semantics[$i] eq "IN_TEX" or $semantics[$i] eq "IN_OUT"){
        print CPP "
  vglCheckContext($variable[$i]".", VGL_GL_CONTEXT);
";
    }
  }

  print CPP "
  GLint _viewport[4];
  glGetIntegerv(GL_VIEWPORT, _viewport);

";

  print CPP "
  static GLuint _f = 0;
  if (_f == 0){
    printf(\"FRAGMENT SHADER\\n====================\\n\");
    _f = vglShaderLoad(GL_FRAGMENT_SHADER, (char*) \"$cpp_read_path$basename\.frag\");
    if (!_f){
      printf(\"%s: %s: Error loading fragment shader.\\n\", __FILE__, __FUNCTION__);
      exit(1);
   }
  }
  ERRCHECK()

  glUseProgram(_f);

";

  
  $i_tex = 0;
  $i_uniform = 0;
  print "==== Input semantics\n";
  for ($i = 0; $i <= $#type; $i++){
    if ($semantics[$i] eq "IN_TEX" or $semantics[$i] eq "IN_OUT"){
      print "semantics[$i] = ".$semantics[$i]."\n";
      for ($j = $i_uniform; $j <= $#uniform; $j++){
        print "j = $j\n";
        if ($uniform[$j]){
          ($type, $unif_type, $unif_variable, $cpp_value) = ProcessGlslUniform($uniform[$i]);
          print "type = $type\n";
          print "unif_variable = $unif_variable\n";
          if ($type =~ m#sampler(2|3)D#){
	    $ndim = $1;
            print CPP "  glActiveTexture(GL_TEXTURE$i_tex);\n";
            print CPP "  glBindTexture(GL_TEXTURE_$ndim"."D, $variable[$i]"."->tex);\n";
            print CPP "  glUniform1i(glGetUniformLocation(_f, \"$unif_variable\"),  $i_tex);\n";
            print CPP "  ERRCHECK()\n";
            print CPP "\n";
	    $i_tex++;
            $i_uniform = $j + 1;
            last;
	  }
	}
      }
    }
  }

  $i_fbo = 0;
  print "==== Output semantics\n";
  for ($i = 0; $i <= $#type; $i++){
    if ($semantics[$i] eq "OUT_FBO" or $semantics[$i] eq "IN_OUT"){
      print "semantics[$i] = ".$semantics[$i]."\n";
      if ($i_fbo == 0){
        $first_framebuffer = $variable[$i];
        print CPP "  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, $first_framebuffer"."->fbo);\n";
        print CPP "  CHECK_FRAMEBUFFER_STATUS()\n";
        print CPP "  ERRCHECK()\n\n";
      }
      else{
        print CPP "  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT$i_fbo"."_EXT, GL_TEXTURE_2D, $variable[$i]"."->tex, 0);\n";
        print CPP "  CHECK_FRAMEBUFFER_STATUS()\n";
      }
      $i_fbo++;
    }
  }

  if ($i_fbo > 1){
    print CPP "
  glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
  GLenum buffers[] = { ";

  for ($i = 0; $i < $i_fbo; $i++){
    print CPP "
    GL_COLOR_ATTACHMENT$i"."_EXT,"
  }

    print CPP " };
  glDrawBuffers($i_fbo, buffers);
  ERRCHECK()

";
  }#endif

  for ($i = 0; $i <= $#uniform; $i++){
    if ($uniform[$i]){
      print "Processing uniform = $uniform[$i]\n";
      ($type, $typef, $variable, $cpp_value) = ProcessGlslUniform($uniform[$i]);
      if ($cpp_value){
        if ($typef =~ m#^(Matrix)#){
          print CPP "  glUniform$typef(glGetUniformLocation(_f, \"$variable\"), 1, 0, $cpp_value);\n";
        }
	else{
          print CPP "  glUniform$typef(glGetUniformLocation(_f, \"$variable\"), $cpp_value);\n";
	}
      }
    }
  }

  if ($first_framebuffer eq ""){
    die "At least one output image is required in $basename\n";
  }

  print CPP "
  glViewport(0, 0, 2*$first_framebuffer->shape[VGL_WIDTH], 2*$first_framebuffer->shape[VGL_HEIGHT]);

      glBegin(GL_QUADS);
          glTexCoord2f( 0.0,  0.0);
          glVertex3f ( -1.0, -1.0, 0.0); //Left  Up

          glTexCoord2f( 1.0,  0.0);
          glVertex3f (  0.0, -1.0, 0.0); //Right Up

          glTexCoord2f( 1.0,  1.0);
          glVertex3f (  0.0,  0.0, 0.0); //Right Bottom

          glTexCoord2f( 0.0,  1.0);
          glVertex3f ( -1.0,  0.0, 0.0); //Left  Bottom
      glEnd();

  glUseProgram(0);

  glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);


";

  for ($i = 0; $i <= $#type; $i++){
    if ($semantics[$i] eq "OUT_FBO" or $semantics[$i] eq "IN_OUT"){
        print CPP "
  if ($variable[$i]"."->has_mipmap){
    glBindTexture(GL_TEXTURE_2D, $variable[$i]"."->tex);
    glGenerateMipmapEXT(GL_TEXTURE_2D);
  }
";
    }
  }

  for ($i = $i_tex - 1; $i > 0; $i--){
    print CPP "
  glActiveTexture(GL_TEXTURE$i_tex);
  glDisable(GL_TEXTURE_2D);

";
  }

  if ($i_fbo > 1){
    print CPP "  glPopAttrib();\n\n";
  }

  print CPP "  glActiveTexture(GL_TEXTURE0);\n";

  for ($i = 0; $i <= $#type; $i++){
    if ($semantics[$i] eq "OUT_FBO" or $semantics[$i] eq "IN_OUT"){
        print CPP "
  vglSetContext($variable[$i]".", VGL_GL_CONTEXT);
";
    }
  }



  print CPP "}\n\n";

  #close(CPP);
}
  
#############################################################################
# Main program
#
#

$USAGE="
Usage:

glsl2cpp  [-o OutputFile] [-p ShadersPath] InputFileList

OutputFile      Source file to which the output will be written. Two files 
                are written with this prefix, a \".cpp\" and a \".h\". 
                It is optional and the default is \"glsl2cpp_shaders\".

ShadersPath     Path to shader files, added to cpp source code before the 
                shader file name. Default is blank.

InputFileList   List of input files. Wildcard characters are allowed.
                Must be placed after the other parameters.

";

print $USAGE;


$nargs = $#ARGV;
$nargs++;        
print "Number of args = $nargs\n";

$foo = '
$lixo = "IN_TEX";
if (!($lixo =~ m#^(IN_TEX|OUT_FBO)$#)){
    print "combina\n";
}
else {
    print "nao combina\n";
}
die "morreu";
';



for ($i=0; $i<$nargs; $i=$i+2) {
  if    ($ARGV[$i] eq "-o") {  
    $output = $ARGV[$i+1] ;
    print ("Output Files: $output.cpp and $output.h\n") ;
  }
  elsif ($ARGV[$i] eq "-p") {  
    $cpp_read_path = $ARGV[$i+1] ;
    print ("Shader files search path: $cpp_read_path\n") ;
  }
  else {
    last;
  }
}

if (!$output){
  $output = "glsl2cpp_shaders";
}


if (!$cpp_read_path){
  $cpp_read_path = "";
}
elsif ($cpp_read_path =~ m#[^/]$#){
  $cpp_read_path = "$cpp_read_path/";
}


$firstInputFile = $i;

unlink("$output.cpp");
unlink("$output.h");

$topMsg = "
/*********************************************************************\
***                                                                 ***
***  Source code generated by glsl2cpp.pl                           ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
\*********************************************************************/
";
open HEAD, ">>", "$output.h";
print HEAD $topMsg;
print HEAD "#include \"vglImage.h\"\n";
close HEAD;
open CPP, ">>", "$output.cpp";
print CPP $topMsg;
print CPP "
#include \"vglImage.h\"
#include \"vglLoadShader.h\"
#include \"vglContext.h\"\n
#include <iostream>
";
close HEAD;
close CPP;



for ($i=$firstInputFile; $i<$nargs; $i++) {
    $fullname = $ARGV[$i];
    print "====================\n";
    print "$ARGV[$i]\n";
    ($a, $b, $c) = fileparse($fullname, ".frag");
    $a or $a = ""; 
    $b or $b = ""; 
    $c or $c = ""; 
    print "Path: $b\n";
    print "Basename: $a\n";
    print "Extenssion: $c\n";
    #print "name: ".basename($fullname)." path: ".dirname($fullname)."\n";
    $basename = $a;

    undef $comment;
    undef @semantics;
    undef @type;
    undef @variable;
    undef @uniform;


    ($comment, $semantics, $type, $variable, $default, $uniform) = ProcessGlslFile($ARGV[$i]);


    PrintCppFile($basename, $comment, $semantics, $type, $variable, $default, $uniform, $output, $cpp_read_path);

}



