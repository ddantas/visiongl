#!/usr/bin/perl -w

use File::Basename;
use common qw(LineStartMultiLineComment
              LineStartSingleLineComment
              lixo);


#############################################################################
# LineStartCleanComments
#
# Throws away comments in begginning of $line
# 
sub LineStartCleanComments { # ($line) {
  my $line = $_[0];

  $multi  = "foo";
  $single = "foo";

  #print "LineStartCleanComments:\n";

  while ($multi or $single){
    ($multi, $line) = LineStartMultiLineComment($line);
    ($single, $line) = LineStartMultiLineComment($line);

    if ($single){
      #print "Single = $single\n";
    }
    if ($multi){
      #print "Multi = $multi\n";
    }
  }

  return $line;
}


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
# LineStartGlobalHeader
#
# Returns the string after the "//" to the first "\n" if found 
# in start of $line, blank string if not found.
# 
sub LineStartGlobalHeader { # ($line) {
  my $line = $_[0];

  if ($line =~ s#^\s*(template)\s*<\s*(typename)\s*(\w*)>##){
    $glo_template = $3;
  }
  else{
    $glo_template = "";
  }
  print "Global header: template = $glo_template\n"; 

  if ($line =~ s#^\s*(__global__)\s*([a-zA-Z_]\w*)\s*([a-zA-Z_]\w*)\s*##){
    $glo_funcname = $3;
  }
  else{
    $glo_funcname = "";
  }
  print "Global header: funcname = $glo_funcname\n"; 

  $line =~ s#^\s*(\(.*\))##;
  print "Global header: paramlist = $1\n"; 
  return ($glo_template, $glo_funcname, $1, $line);
}

#############################################################################
# LineStartParenthesis
#
# Returns the string after the "(" 
# in start of $line, blank string if not found.
# 
sub LineStartParenthesis { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*\((.*)##;
  return $1;
}

#############################################################################
# LineStartParenthesis
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
# Valid semantic bindincs are IO_PBO, IN_PBO, OUT_PBO and IN_TEX.
# 
sub LineStartSemantics { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([a-zA-Z_]\w*)\s*:##;
  $semantics = $1;
  if ($semantics){
    if (!($semantics =~ m#^(IO_PBO|IN_PBO|OUT_PBO|IN_TEX)$#))
    {
      die "Semantic $semantics invalid. Valid semantics are (IO_PBO|IN_PBO|OUT_PBO|IN_TEX)\n";
    }
  }
  else{
    $semantics = "";
  }
  return ($semantics, $line);
}

#############################################################################
# LineStartTypeStar
#
# Returns the string after the datatype and, * if present,
# in start of $line, blank string if not found.
# 
sub LineStartTypeStar { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([a-zA-Z_]\w*[\s*\*]*)##;
  return ($1, $line);
}

#############################################################################
# LineStartType
#
# Returns the datatype in start of $line, blank string if not found.
# 
sub LineStartType { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([a-zA-Z_]\w*)##;
  return ($1, $line);
}

#############################################################################
# LineStartExpression
#
# Returns the cpp expression in the start of the line,
# blank string if not found.
# 
sub LineStartExpression { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([^,\(\)]*)\s*##;
  return ($1, $line);
}

#############################################################################
# LineStartVariable
#
# Returns the variable name 
# in start of $line, blank string if not found.
# 
sub LineStartVariable { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([a-zA-Z_]\w*)##;
  return ($1, $line);
}

#############################################################################
# LineStartStarVariable
#
# Returns the "*" and variable name
# in start of $line, blank string if not found.
# 
sub LineStartStarVariable { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([\*\s*]*[a-zA-Z_]\w*)##;
  return ($1, $line);
}

#############################################################################
# LineStartDefault
#
# Returns the string, after the variable
# in start of $line, that defines its default value, blank string if not found.
# 
sub LineStartDefault { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*(=\s*-?\s*[\.\w]*)##;
  return ($1, $line);
}

#############################################################################
# LineStartSeparator
#
# Returns the string after the first "," or ")" found
# in start of $line, blank string if not found.
# 
sub LineStartSeparator { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*(,|\))##;
  return ($1, $line);
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
# LineStartExecution
#
# Returns the string that contains the execution configuration, that is,
# everything between <<< and >>> including it.
# 
sub LineStartExecution { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*(<<<\s*[\w->\[\]]+\s*(,\s*[\w->\[\]]*\s*){0,2}>>>)##;
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

    if ($type =~ m#([234])#){
      $type_gluniform = "$1";
    }
    else{
      $type_gluniform = "1";
    }

    if ($type =~ m#^(float|vec)#){
      $type_gluniform .= "f";
    }
    elsif ($type =~ m#^(int|ivec)#){
      $type_gluniform .= "i";
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
# ProcessKernelHeader
#
# Receives as input a .kernel header line and breaks it
#
#
sub ProcessKernelHeader { # ($line) {
  my $line          = $_[0];

  my $separator;
  my $execution;
  my $semantics;
  my $type;
  my $variable;
  my $default;

  ($execution, $line) = LineStartExecution($line);
  if (!$execution){
    die "Error: Start-of-line execution configuration not found\n";
  }
  else{
    print "After eliminating execution configuration:\n$line\n";
  }  

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
  return ($execution, $semantics, $type, $variable, $default);

}

#############################################################################
# ProcessKernelGlobalParams
#
# Receives as input a .kernel global function parameter list 
# and breaks it.
#
sub ProcessKernelGlobalParams { # ($line) {
  my $line          = $_[0];

  my $separator = "";
  my $glo_type = "";
  my $glo_variable = "";

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

    ($glo_type[$i], $line) = LineStartTypeStar($line);
    if (!$glo_type[$i]){
      print "Start-of-line type not found\n";
    }
    else{
      #print "After eliminating type:\n$line\n";
    }

    ($glo_variable[$i], $line) = LineStartVariable($line);
    if (!$glo_variable[$i]){
      print "Start-of-line variable not found\n";
    }
    else{
      #print "After eliminating variable:\n$line\n";
    }

    ($separator, $line) = LineStartSeparator($line);

    if ($separator and $separator eq ","){
      #print "Variable list continues\n";
    }

    print "Global type     [$i]: $glo_type[$i]\n";
    print "Global variable [$i]: $glo_variable[$i]\n";

    $i++;
  }
  return ($glo_type, $glo_variable);

}

#############################################################################
# ProcessKernelCaller
#
# Receives as input a .kernel caller line and breaks it
#
#
sub ProcessKernelCaller { # ($line) {
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

    ($expression[$i], $line) = LineStartExpression($line);
    if (!$expression[$i]){
      print "Start-of-line expression not found\n";
    }
    else{
      #print "After eliminating expression:\n$line\n";
    }

    ($separator, $line) = LineStartSeparator($line);

    if ($separator and $separator eq ","){
      #print "Variable list continues\n";
    }

    print "Express. [$i]: $expression[$i]\n";

    $i++;
  }
  return ($expression);

}

#############################################################################
# ProcessKernelFile
#
# Receives as input a .kernel filename and analyses it.
#
#
sub AppendFile { # ($inFilename, $outFilename) {
  my $inFilename      = $_[0];
  my $outFilename     = $_[1];

  open(IN_FILE, $inFilename);
  @list = <IN_FILE>;
  $line = join("", @list);

  open OUT_FILE, ">>", $outFilename;
  print OUT_FILE $line;
  close OUT_FILE;

}

#############################################################################
# ProcessKernelFile
#
# Receives as input a .kernel filename and analyses it.
#
#
sub ProcessKernelFile { # ($filename) {
  my $filename      = $_[0];
  
  my $comment; 
  my $semantics; 
  my $type;
  my $variable;
  my $default;
  my $expression;

  undef $comment; 
  undef $semantics; 
  undef $type;
  undef $variable;
  undef $expression;

  my $glo_template;
  my $glo_funcname;
  my $glo_paramlist;
  my $glo_type;
  my $glo_variable;

  open(IN_KERNEL, $filename);
  @list = <IN_KERNEL>;
  $line = join("", @list);

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
  ($execution, $semantics, $type, $variable, $default) = ProcessKernelHeader($perl_header);

  ($perl_caller, $line) = LineStartHeader($line);
  if (!$perl_caller){
    print "Error: start-of-file caller not found\n";
    return;
  }
  else{
    print "Writing this caller:\n$perl_caller\n";
  }
  ($expression) = ProcessKernelCaller($perl_caller);

  $line = LineStartCleanComments($line);

  ($glo_template, $glo_funcname, $glo_paramlist, $line) = LineStartGlobalHeader($line);
  if (!$glo_funcname){
    print "Error: global function name not found\n";
    return;
  }
  else{
    print "Writing this global function name:\n$glo_funcname\n";
  }
  ($glo_type, $glo_variable) = ProcessKernelGlobalParams($glo_paramlist);


  

  close(IN_KERNEL);
  
  return  ($comment, $semantics, $type, $variable, $default, $execution, $expression, $glo_template, $glo_funcname, $glo_type, $glo_variable);
}

#############################################################################
# PrintCudaFile
#
# Receives as input the data extracted from the source code
# and generates CPP wrapper function
#
#
sub PrintCudaFile { # ($basename, $comment, $semantics, $type, $variable, $default, $uniform, $output, $cpp_read_path) {
  my $basename      = $_[0];
  my $comment       = $_[1];
  my $semantics     = $_[2];
  my $type          = $_[3];
  my $variable      = $_[4];
  my $default       = $_[5];
  my $execution     = $_[6];
  my $expression    = $_[7];
  my $glo_template  = $_[8];
  my $glo_funcname  = $_[9];
  my $glo_type      = $_[10];
  my $glo_variable  = $_[11];
  my $output        = $_[12];
 
  my $i;
  my $j;
  my $first_framebuffer = "";

  print "Will write to $output.cu and $output.h\n";

  open CUDA, ">>", "$output.cu";
  open HEAD, ">>", "$output.h";


  #print CUDA "$comment\n";
  print CUDA "\n";
  print HEAD "$comment\n";

  print CUDA "void $basename(";
  print HEAD "void $basename(";
  for ($i = 0; $i <= $#type; $i++){
    print CUDA "$type[$i] $variable[$i]";
    print HEAD "$type[$i] $variable[$i]" . ($default[$i] or "");
    if ($i < $#type){
      print CUDA ", ";
      print HEAD ", ";
    } 
  }
  print CUDA "){\n";
  print HEAD ");\n\n";

  my $first_var = "";
  my $var = "";
  print "==== Input semantics\n";
  for ($i = 0; $i <= $#type; $i++){
    if ($semantics[$i] eq "IN_TEX" or $semantics[$i] eq "IO_PBO" or 
        $semantics[$i] eq "IN_PBO" or $semantics[$i] eq "OUT_PBO"   ){
      if ($first_var eq ""){
        $first_var = $variable[$i];
      }
      $var = $variable[$i];
      print "semantics[$i] = ".$semantics[$i]."\n";
      print CUDA "    if (!".$var."){\n";
      print CUDA "      printf(\"$basename: Error: ".$var." parameter is null in file '%s' in line %i.\\n\",\n";
      print CUDA "              __FILE__, __LINE__);\n";
      print CUDA "      exit(1);\n";
      print CUDA "    }\n";
      if ($semantics[$i] eq "IN_TEX" or $semantics[$i] eq "IO_PBO" or 
          $semantics[$i] eq "IN_PBO"                                  ){
        print CUDA "    vglCheckContext(".$var.", VGL_CUDA_CONTEXT);\n";
      }
      if ($semantics[$i] eq "OUT_PBO"){
        print CUDA "    vglCheckContextForOutput(".$var.", VGL_CUDA_CONTEXT);\n";
      }
      print CUDA "    if (!".$var."->cudaPtr){\n";
      print CUDA "      printf(\"$basename: Error: ".$var."->cudaPtr is null in file '%s' in line %i.\\n\",\n";
      print CUDA "              __FILE__, __LINE__);\n";
      print CUDA "      exit(1);\n";
      print CUDA "    }\n";
      print CUDA "\n";
    }
  }

  $type_opencv[0] = "IPL_DEPTH_8U";
  $type_cpp[0] = "unsigned char";
  
  my $a_type;
  my $a_cpptype;
  print CUDA "\n";
  print CUDA "    switch ($first_var->depth){\n";
  for ($i = 0; $i <= $#type_opencv; $i++){
    print CUDA "      case (".$type_opencv[$i]."):\n";
    print CUDA "        $glo_funcname$execution(";
    $a_cpptype = $type_cpp[$i];
    for ($j = 0; $j <= $#glo_type; $j++){
      $a_type = $glo_type[$j];
      if ($a_type =~ m#^$glo_template#){
        $a_type =~ s#^$glo_template#$a_cpptype#;
        print CUDA "(".$a_type.")";
      }
      print CUDA $expression[$j];
      if ($j < $#glo_type){
        print CUDA ", ";
      }
    }
    print CUDA ");\n";
    print CUDA "        break;\n";
  }
  print CUDA "      default:\n";
  print CUDA "        printf(\"$basename: Error: unsupported img->depth = %d in file '%s' in line %i.\\n\",\n";
  print CUDA "                ".$first_var."->depth, __FILE__, __LINE__);\n";
  print CUDA "        exit(1);\n";
  print CUDA "    }\n";
  print CUDA "\n";

  for ($i = 0; $i <= $#type; $i++){
    if ($semantics[$i] eq "IO_PBO" or $semantics[$i] eq "OUT_PBO"){
      $var = $variable[$i];
      print CUDA "    vglSetContext(".$var.", VGL_CUDA_CONTEXT);\n";
    }
  }

  print CUDA "}\n\n";

  close(CUDA);
  close(HEAD);
}
  
#############################################################################
# Main program
#
#

$USAGE="
Usage:

kernel2cu  [-o OutputFile] InputFileList

OutputFile      Source file to which the output will be written. Two files 
                are written with this prefix, a \".cu\" and a \".h\". 
                It is optional and the default is \"kernel2cu_shaders\".

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
    print ("Output Files: $output.cu and $output.h\n") ;
  }
  else {
    last;
  }
}

if (!$output){
  $output = "kernel2cu_shaders";
}

$firstInputFile = $i;

system "rm $output.kernel";
system "rm $output.cu";
system "rm $output.h";

$topMsg = "
/*********************************************************************\
***                                                                 ***
***  Source code generated by kernel2cu.pl                          ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
\*********************************************************************/
";
open HEAD, ">>", "$output.h";
print HEAD $topMsg;
open HEAD, ">>", "$output.h";
print HEAD "#include \"vglImage.h\"\n";
close HEAD;
open CUDA, ">>", "$output.cu";
print CUDA $topMsg;
print CUDA "
#include \"vglImage.h\"
#include \"vglLoadShader.h\"
#include \"vglContext.h\"\n
#include <iostream>

//kernels
";
close CUDA;



for ($i=$firstInputFile; $i<$nargs; $i++) {
    $fullname = $ARGV[$i];
    print "====================\n";
    print "$ARGV[$i]\n";
    ($a, $b, $c) = fileparse($fullname, ".kernel");
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
    undef @default;
    undef @execution;
    undef @expression;
    undef @glo_template;
    undef @glo_funcname;
    undef @glo_type;
    undef @glo_variable;
    undef @output;


    AppendFile($ARGV[$i], "$output.cu");

    ($comment, $semantics, $type, $variable, $default, $execution, $expression, $glo_template, $glo_funcname, $glo_type, $glo_variable) = ProcessKernelFile($ARGV[$i]);

    PrintCudaFile($basename, $comment, $semantics, $type, $variable, $default, $execution, $expression, $glo_template, $glo_funcname, $glo_type, $glo_variable, $output);

}




