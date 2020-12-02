#!/usr/bin/perl -w

BEGIN {push @INC, '.'}

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
# LineStartHeader
#
# Returns the header of the program, that is, the 
# text starting with __kernel until the character before {
# 
sub LineStartHeader { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*((__kernel)\s*(void)[\s\S]*?)\{##; # *? is ungreedy match
  $result = $1;
  $result =~ s#\s+# #g;

  return ($result, $line);
}

#############################################################################
# LineStartSemantics
#
# Returns the string after the semantic binding and the ":"
# in start of $line, blank string if not found.
# Valid semantic bindings are __read_only, __write_only and __constant
# 
sub LineStartSemantics { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*(__read_only|__write_only|__read_write|__constant)\s*##;
  $semantics = $1;
  if ($semantics){
    if (!($semantics =~ m#^(__read_only|__write_only|__read_write|__constant)$#))
    {
      print "Semantic $semantics invalid. Valid semantics are (__read_only|__write_only|__read_write|__constant)\n";
    }
  }
  else{
    $semantics = "";
  }
  return ($semantics, $line);
}

#############################################################################
# LineStartDirective
#
# Returns the string after the directive
# in start of $line, blank string if not found.
# Valid directives are SCALAR and ARRAY
# 
sub LineStartDirective { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([a-zA-Z_]\w*)\s*##;
  my $result = $1;
  print "Directive type >>>$result<<<\n";
  if ($result){
    if (!($result =~ m#^(SCALAR|ARRAY)$#))
    {
      die "Directive $result invalid. Valid directives are (SCALAR|ARRAY)\n";
    }
  }
  else{
    $result = "";
  }
  return ($result, $line);
}

#############################################################################
# LineStartSize
#
# Returns the string between [] in 
# start of $line, blank string if not found.
# 
sub LineStartSize { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*\[\s*(.*)\s*\]##;
  my $result = $1;
  print "Found size >>>$result<<<\n";
  return ($result, $line);
}

#############################################################################
# LineStartMain
#
# Returns the string that contains the expression __kernel void.
# 
sub LineStartMain { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*((__kernel)\s*(void)\s*)##;
  return ($1, $line);
}

#############################################################################
# ProcessClDirectives
#
# Receives as input an array of CL directives and links them to the respective variables
#
# A CL Directive purpose is to specify the size of arrays passed as parameters to 
# a kernel. The size is needed by the wrapper to calculate the buffer size. The buffer
# size is passed as parameter to clCreateBuffer and clEnqueueWriteBuffer.
#
sub ProcessClDirectives { # ($directives, $variable) {
  my $directives   = $_[0];
  my $variable     = $_[1];

  my $i = 0;
  my $j = 0;

  my $result_directive;
  my $result_variable;
  my $result_size;


  print "ProcessClDirectives directives size = $#directives\n";
  print "ProcessClDirectives variable size   = $#variable\n";

  for ($i = 0; $i <= $#variable; $i++){
    $is_array[$i] = 0;
    $size[$i]     = "1";
    print "-----------Is array [$i]: $is_array[$i]\n";
    print "-----------Size     [$i]: $size[$i]\n";

  }


  print "ProcessClDirectives directives size = $#directives\n";
  die;

  for ($i = 0; $i <= $#directives; $i++){
    print "###########################################################\n";
    print "Processing directive comment[$i]: >>>>$directives[$i]<<<<\n";

    ($result_directive, $directives[$i]) = LineStartDirective($directives[$i]);
    print "1 Directive type = >>>$result_directive<<<\n";
    if (!$result_directive){
      die "Error: Start-of-line directive (SCALAR|ARRAY) not found\n";
    }
    else{
      print "After eliminating directive (SCALAR|ARRAY):\n$directives[$i]\n";
    }

    ($result_variable, $directives[$i]) = LineStartVariable($directives[$i]);
    print "2 Variable name = >>>$result_variable<<<\n";
    if (!$result_variable){
      die "Error: Start-of-line variable name not found\n";
    }
    else{
      print "After eliminating variable name:\n$directives[$i]\n";
    }

    print "3 Directive type = >>>$result_directive<<<\n";
    if ($result_directive eq "ARRAY"){
      print "Searching size of ARRAY type directive\n";
      ($result_size, $directives[$i]) = LineStartSize($directives[$i]);
      if (!$result_size){
        die "Error: Start-of-line size value not found\n";
      }
      else{
        print "After eliminating size value:\n$directives[$i]\n";
      }
    }

    print "FOR STARTING $#variable\n";
    for ($j = 0; $j <= $#variable; $j++){
      print ">>>$result_variable<<< == >>>$variable[$j]<<<\n";
      if ($result_variable eq $variable[$j]){
        if ($result_directive eq "ARRAY"){
          $is_array[$j] = 1;
          $size[$j] = $result_size;
          print "Is array [$j]: $is_array[$j]\n";
          print "Size     [$j]: $size[$j]\n";
        }
      }
    }
  }

    for ($j = 0; $j <= $#variable; $j++){
          print "Is array [$j]: $is_array[$j]\n";
          print "Size     [$j]: $size[$j]\n";
    }

  return ($is_array, $size);

}

#############################################################################
# ProcessClDirective
#
# Receives as input a CL directive and returns directive type, variable name and size.
#
# A CL Directive purpose is to specify the size of arrays passed as parameters to 
# a kernel. The size is needed by the wrapper to calculate the buffer size. The buffer
# size is passed as parameter to clCreateBuffer and clEnqueueWriteBuffer.
#
sub ProcessClDirective { # ($directive) {
  my $directive   = $_[0];

  my $i = 0;
  my $j = 0;

  my $result_directive;
  my $result_variable;
  my $result_size;
  my $result_isarray;


    print "ProcessingClDirective started###########################################################\n";
    print "ProcessClDirective directives size = $#directive\n";
    print "Processing directive comment[$i]: >>>>$directive<<<<\n";

    ($result_directive, $directive) = LineStartDirective($directive);
    print "1 Directive type = >>>$result_directive<<<\n";
    if (!$result_directive){
      die "Error: Start-of-line directive (SCALAR|ARRAY) not found\n";
    }
    else{
      print "After eliminating directive (SCALAR|ARRAY):\n$directive\n";
    }

    ($result_variable, $directive) = LineStartVariable($directive);
    print "2 Variable name = >>>$result_variable<<<\n";
    if (!$result_variable){
      die "Error: Start-of-line variable name not found\n";
    }
    else{
      print "After eliminating variable name:\n$directive\n";
    }

    print "3 Directive type = >>>$result_directive<<<\n";
    if ($result_directive eq "ARRAY"){
      print "Searching size of ARRAY type directive\n";
      ($result_size, $directive) = LineStartSize($directive);
      if (!$result_size){
        die "Error: Start-of-line size value not found\n";
      }
      else{
        print "After eliminating size value:\n$directive\n";
      }
    }

    $result_isarray = 0;
    if ($result_directive eq "ARRAY"){
      $result_isarray = 1;
    }

  return ($result_isarray, $result_variable, $result_size);

}

#############################################################################
# IsVariableImage
#
# Receives as input the semantics and type. Returns true if refers to image type
# and false otherwise.
#
sub IsVariableImage { # ($semantics, $type) {
  my $semantics     = $_[0];
  my $type          = $_[1];

  if ( ($semantics eq "__read_only") or 
       ($semantics eq "__write_only") or 
        ($semantics eq "__read_write")   )
    {
    if ( ($type eq "image2d_t") or 
         ($type eq "image3d_t") or 
         ($type eq "char*")             )
    {
      return 1;
    }
  }
  return 0;
}

#############################################################################
# IsVariableArray
#
# Receives as input the semantics and type. Returns true if refers to array type
# and false otherwise.
#
#sub IsVariableArray { # ($semantics, $type) {
#  my $semantics     = $_[0];
#  my $type          = $_[1];

#  if ( ($semantics eq "__constant") ){
#    if ( ($type =~ m#\*$#) ){
#      return 1;
#    }
#  }
#  return 0;
#}


#############################################################################
# ProcessClHeader
#
# Receives as input a CL header line and breaks it
#
#
sub ProcessClHeader { # ($line) {
  my $line          = $_[0];

  my $result;
  my $kernelname;
  my $separator;
  my $semantics;
  my $type;
  my $output;
  my $variable;

  ($result, $line) = LineStartMain($line);
  if (!$result){
    die "Error: Start-of-line \"__kernel void\" not found\n";
  }
  else{
    #print "After eliminating \"__kernel void\":\n$line\n";
  }  

  ($kernelname, $line) = LineStartVariable($line);
  if (!$kernelname){
    die "Error: Start-of-line kernel name identifier not found\n";
  }
  else{
    #print "After eliminating kernel name \"$kernelname\":\n$line\n";
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
      print "Start-of-line semantics not found\n";
    }
    else{
      print "After eliminating semantics:\n$line\n";
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
  print "variable size ====== $#variable\n";

  return ($semantics, $type, $variable, $default, $kernelname);

}

#############################################################################
# ProcessClFile
#
# Receives as input a CL filename and generates CPP wrapper function
#
#
sub ProcessClFile { # ($filename, $output, $cpp_read_path) {
  my $filename      = $_[0];
  my $output        = $_[1];
  my $cpp_read_path = $_[2];
  
  my $comment; 
  my $semantics; 
  my $type;
  my $variable;
  my $default;
  my $uniform;
  my $dircomment;
  my $is_array;
  my $size;

  print "directive size = $#dircomment\n";

  undef $comment; 
  undef $semantics; 
  undef $type;
  undef $variable;
  undef $default;
  undef $uniform;
  undef $dircomment;
  undef $is_array;
  undef $size;

  print "directive size = $#dircomment\n";
  #print "directive 0 = $dircomment[0]\n";
  #print "directive 1 = $dircomment[1]\n";
  print "commment size = $#comment\n";
  print "semantics size = $#semantics\n";
  print "type size = $#type\n";
  print "variable size = $#variable\n";

  open CL, $filename;
  @list = <CL>;
  $line = join("", @list);
  close CL;

  ($comment, $line) = LineStartMultiLineComment($line);
  if (!$comment){
    print "Start-of-file comment not found\n";
  }
  else{
    print "Found this comment:\n$comment\n";
  }  

  print "directive size = $#directive\n";

  my $i = 0;
  my $catdirectives = "";
  do {
    ($dircomment[$i], $line) = LineStartSingleLineComment($line);
    print "directive[$i] = >>$dircomment[$i]<< size = >>$#dircomment<<\n";
    if ($dircomment[$i]){
      print ("Found directive: $dircomment[$i]\n");
      ($dirisarray[$i], $dirvar[$i], $dirsize[$i]) = ProcessClDirective($dircomment[$i]);
      print ("ProcessClFile result = <$dirisarray[$i]> <$dirvar[$i]> <$dirsize[$i]>\n");
    }
    else{
      #print ("Directive not found\n");
    }
    $i++;
  }
  while ($dircomment[$i-1]);
  delete $dircomment[$i-1];
  print "DIRECTIVE SIZE AFTER LineStartSingleLineComment: $#dircomment\n";

  print "Starting for to print directives found\n";
  for($i = 0; $i <= $#dirvar; $i++){
      print ("ProcessClFile result = <$dirisarray[$i]> <$dirvar[$i]> <$dirsize[$i]>\n");  
  }

  ($perl_header, $line) = LineStartHeader($line);
  if (!$perl_header){
    print "Error: start-of-file header not found\n";
    return;
  }
  else{
    print "Found this header:\n$perl_header\n";
  }

  ($semantics, $type, $variable, $default, $kernelname) = ProcessClHeader($perl_header);
  print "variable sizeeeeee = $#variable\n";
  print "ProcessClFile dircomment size = $#dircomment\n";


  print "FOR STARTING $#variable\n";
  for($i = 0; $i <= $#variable; $i++){
    $is_array[$i] = 0;
    $size[$i]     = 1;
  }
  print "FOR STARTING $#variable\n";
  for($i = 0; $i <= $#variable; $i++){
    for ($j = 0; $j <= $#dirvar; $j++){
       print ">>>$variable[$i]<<< == >>>$dirvar[$j]<<<\n";
       if ($variable[$i] eq $dirvar[$j] && $dirisarray[$j]){
         $is_array[$i] = $dirisarray[$j];
         $size[$i]     = $dirsize[$j];
         print "Is array [$i]: $is_array[$i]\n";
         print "Size     [$i]: $size[$i]\n";
       }
     }
   }

  my $j = 0;

  if ($kernelname ne $basename){
    die "Error: File basename \"$basename\" != kernel name \"$kernelname\". Please rename your kernel to \"$basename\"\n";
  }
  #$line = LineStartCleanComments($line);

  print "\n";
  for($i = 0; $i <= $#variable; $i++){
    print "Semantics[$i]: " . ($semantics[$i] or "") . "\n";
    print "Type     [$i]: $type[$i]\n";
    print "Variable [$i]: $variable[$i]\n";
    print "Default  [$i]: " . ($default[$i] or "") . "\n";
    print "Array?   [$i]: $is_array[$i]\n";
    print "Size     [$i]: $size[$i]\n";
  }

  return  ($comment, $semantics, $type, $variable, $is_array, $size);
}



#############################################################################
# PrintFunctions
#
# Receives as input a handle to a file and prints common functions
#
#
sub PrintFunctions { # ($filename) {
  my $filename      = $_[0];

  open CPP, ">>", "$filename";

  $str = "
string* getValue(int arg_position, int argc, char* argv[])
{
    if (arg_position+1 > argc || strcmp(string(argv[arg_position+1]).substr(0,2).c_str(),\"--\") == 0)
    {
        printf(LACKING_VALUE_FOR_ARG,argv[arg_position]);
    }
    else
    return new string(argv[arg_position+1]);
}

//troca uma substring por outra substring
string replaceinString(std::string str, std::string tofind, std::string toreplace)
{
    size_t position = 0;
    for ( position = str.find(tofind); position != std::string::npos; position = str.find(tofind,position) )
    {
        str.replace(position ,1, toreplace);
    }
    return(str);
}

//Converte uma string contendo um array, em um array de float
void convertStringToArray(string array,float* convolution_window,int size)
{
    array = array.substr(1,array.length()-2);
    array = replaceinString(array,\" \",\"\");

    char* value = strtok((char*)array.c_str(),\",\");
    for (int i = 0; i < size; i++)
    {
        float f;
        if(sscanf(value, \"%%f\", &f)  == -1 )
        {
            printf(WRONG_TYPE_TO_ARG);
            printf(\"argument: window_convolution\\n\");
            printf(\"erro reading vector position %%d\\n\",i+1);            
            exit(1);
        }
        convolution_window[i] = f;
        value = strtok(NULL,\",\");
    }
}

";

  print CPP $str;
  close CPP;

}

#############################################################################
# PrintProcessArgs
#
# Generates ProcessArgs function
#
#
sub PrintProcessArgs { # ($basename, $comment, $semantics, $type, $variable, $default, $is_array, $size, $output, $cpp_read_path) {
  my $basename      = $_[0];
  my $comment       = $_[1];
  my $semantics     = $_[2];
  my $type          = $_[3];
  my $variable      = $_[4];
  my $default       = $_[5];
  my $is_array      = $_[6];
  my $size          = $_[7];
  my $output        = $_[8];
  my $cpp_read_path = $_[9];

  my $i;

  my $filename = "$output/$basename.cpp";

  my $firstPrint = 1;

  open CPP, ">>", "$filename";

  print CPP "
void processArgs(int argc, char* argv[])
{
  if (argc > 1)
  {
    for (int i = 1; i < argc; i++)
    {
      string arg = string(argv[i]);
";


  for ($i = 0; $i <= $#type; $i++){
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      print CPP "      ";
      if ($firstPrint){
        $firstPrint = 0;
      } 
      else{
        print CPP "else ";
      }
      print CPP "if (strcmp(arg.c_str(), \"--$variable[$i]\" == 0)
      {
        string value = *getValue(i, argc, argv);
        $variable[$i] = value;
        isset_$variable[$i] = 1;
      }
";
    }
    elsif ( !($is_array[$i]) )
    {
      print CPP "      ";
      if ($firstPrint){
        $firstPrint = 0;
      } 
      else{
        print CPP "else ";
      }
      print CPP "if (strcmp(arg.c_str(), \"--$variable[$i]\" == 0)
      {
        string value = *getValue(i, argc, argv);
        if (sscanf(value.c_str(), \"%d\", &$variable[$i]) != 1)
        {
          printf(WRONG_TYPE_TO_ARG);
          exit(1);
	}
        isset_$variable[$i] = 1;
      }
";
    }
  }

  print CPP 
"      else
      {
        printf(WRONG_USAGE);
        exit(1);
      }
    }
  }
}
";

  $firstPrint = 1;

  print CPP "
void processArrayArgs(int argc, char* argv[])
{
  if (argc > 1)
  {
    for (int i = 1; i < argc; i++)
    {
      string arg = string(argv[i]);
";


  for ($i = 0; $i <= $#type; $i++){
    if ($is_array[$i])
    {
      print CPP "      ";
      if ($firstPrint){
        $firstPrint = 0;
      } 
      else{
        print CPP "else ";
      }
      print CPP "if (strcmp(arg.c_str(), \"--$variable[$i]\" == 0)
      {
        string value = *getValue(i, argc, argv);
        $variable[$i] = (float*)malloc(sizeof(float) * ($size[$i]));
        convertStringToArray(value, $variable[$i], ($size[$i])
        isset_$variable[$i] = 1;
      }
";
    }
  }

  print CPP 
"      else
      {
        printf(WRONG_USAGE);
        exit(1);
      }
    }
  }
}
";



  close CPP;

}


#############################################################################
# PrintCppFile
#
# Receives as input a CL filename and generates CPP wrapper function
#
#
sub PrintCppFile { # ($basename, $comment, $semantics, $type, $variable, $default, $is_array, $size, $output, $cpp_read_path) {
  my $basename      = $_[0];
  my $comment       = $_[1];
  my $semantics     = $_[2];
  my $type          = $_[3];
  my $variable      = $_[4];
  my $default       = $_[5];
  my $is_array      = $_[6];
  my $size          = $_[7];
  my $output        = $_[8];
  my $cpp_read_path = $_[9];

  my $i;

  my $filename = "$output/$basename.cpp";

  print "Will write to $filename\n";
  unlink($filename);
  open CPP, ">>", "$filename";

  print CPP "
/**********************************************************************
***                                                                 ***
***  Source code generated by cl2cloud.pl                           ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
**********************************************************************/
";
  print CPP "
#define _CRT_SECURE_NO_WARNINGS

#include <visiongl.h>

#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>

#include <fstream>
#include <string>

#define NOT_ENOUGH_ARGS_MSG \"Not enough arguments provided.\\n\"
#define WRONG_USAGE \"Wrong usage, you must first add an argument for execution.\\n\"
#define LACKING_VALUE_FOR_ARG \"Missing value for %%s argument\\n\"
#define WRONG_TYPE_TO_ARG \"Wrong type in an argument\\n\"
#define MISSING_SETUP_ARGS_BEFORE \"You forgot to set some arguments before calling this argument: %%s\\n\"

using namespace std;

";

  for ($i = 0; $i <= $#type; $i++){
    #print ">>>$type[$i]<<< becomes ";
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      print CPP "string _$variable[$i];\n";
      print CPP "int _isset_$variable[$i];\n";
    }
    else{
      #$type[$i] =~ s#^\s*([a-zA-Z_][a-zA-Z0-9_]*)##;
      #$type[$i] = $1;
      print CPP "$type[$i] _$variable[$i];\n";
      print CPP "int _isset_$variable[$i];\n";
    }
    print CPP "\n";
  }

  print CPP "$comment\n";

  PrintFunctions($filename);
  PrintProcessArgs($basename, $comment, $semantics, $type, $variable, $default, $is_array, $size, $output, $cpp_read_path);

  open CPP, ">>", "$filename";

  

  print CPP "
int main(int argc, char* argv[])
{
  processArgs(argc, argv);
  processArrayArgs(argc, argv);

";

  ##############################
  # Printing input values
  print CPP "  // Printing input values\n";
  for ($i = 0; $i <= $#type; $i++){
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      print CPP "  printf(\"_$variable[$i]: %%s\\n\", _$variable[$i].c_str());";
    }
    elsif ($is_array[$i])
    {
      print CPP "
  printf(\"_$variable[$i]: [\");
  for (int i = 0; i < $size[$i]; i++)
  {
    if (i > 0) printf(\", \");
    printf(\"%f\", $variable[$i]"."[i]);
  }
  printf(\"]\\n\");
";    
    }
    else
    {
      print CPP "  printf(\"_$variable[$i]: %%s\\n\", _$variable[$i]);";
    }
    print CPP "\n";
  }

  ##############################
  # Checking input values
  print CPP "  // Checking input values\n";

  for ($i = 0; $i <= $#type; $i++){
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      print CPP 
"  if (!isset_$variable[$i])
  {
    printf(\"Missing $variable[$i] image path, try adding two parameters as in --$variable[$i] path/image.ext\\n\");
    exit(1);
  }
";
    }
    elsif ($is_array[$i])
    {
      print CPP 
"  if (!isset_$variable[$i])
  {
    printf(\"Missing $variable[$i] array, try adding two parameters as in --$variable[$i] \\\"[.1, .2, .3]\\\"\\n\");
    exit(1);
  }
";
    }
    else
    {
      print CPP 
"  if (!isset_$variable[$i])
  {
    printf(\"Missing $variable[$i] parameter, try adding two parameters as in --$variable[$i] value\\n\");
    exit(1);
  }
";
    }
    print CPP "\n";
  }


  ##############################
  # Initializing contexts
  print CPP "  // Initializing contexts\n";

  print CPP 
"  vglInit(30,30);
  vglClInit();

";

  ##############################
  # Declaring VglImage* variables
  print CPP "  // Declaring VglImage* variables\n";
  $some_input_image = -1;
  for ($i = 0; $i <= $#type; $i++){
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      if ( ($semantics[$i] eq "__read_only") or 
           ($semantics[$i] eq "__read_write")   )
      {
        print CPP "  VglImage* img_$variable[$i] = vglLoadImage((char*) $variable[$i].c_str(),1);\n";
        $some_input_image = $i;
      }
      else # $semantics[$i] eq "__write_only"
      {
        print CPP "  VglImage* img_$variable[$i] = NULL;\n"
      }
    }
  }

  for ($i = 0; $i <= $#type; $i++){
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      if ( ($semantics[$i] eq "__read_only") or 
           ($semantics[$i] eq "__read_write")   )
      {
      print CPP "
  if (img_$variable[$i]->nChannels == 3)
  {
    vglImage3To4Channels(img_$variable[$i]);
  }
";
      }
    }
  }

  for ($i = 0; $i <= $#type; $i++){
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      if ( ($semantics[$i] eq "__write_only") )
      {
      print CPP "  img_$variable[$i] = vglCreateImage(img_$variable[$some_input_image]);\n";
      }
    }
  }
  print CPP "\n";

  ##############################
  # Shader call
  print CPP "  // Shader call\n";
  print CPP "  $basename(";
  for ($i = 0; $i <= $#type; $i++){
    if ($i > 0)
    { 
      print CPP ", ";
    }
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      print CPP "img_$variable[$i]";
    }
    else
    {
      print CPP "$variable[$i]";
    }
  }
  print CPP ");\n";

  ##############################
  # Saving result
  print CPP "  // Saving result";
  for ($i = 0; $i <= $#type; $i++){
    if ( IsVariableImage($semantics[$i], $type[$i]) )
    {
      if ( ($semantics[$i] eq "__write_only") or
           ($semantics[$i] eq "__read_write")    )
      {
        print CPP "
  vglCheckContext(img_$variable[$i], VGL_RAM_CONTEXT);
  cvSaveImage($variable[$i].c_str(), img_$variable[$i]->ipl);
"
      }
    }
  }
  print CPP "\n";
  print CPP "  return 0;\n";
  print CPP "}\n";

  close CPP;
}
  
#############################################################################
# Main program
#
#

$USAGE="
Usage:

cl2cloud  [-o OutputFolder] InputFileList

OutputFolder    Folder where source files will be written. 
                It is optional and the default is \"src/cloud\".

InputFileList   List of input files. Wildcard characters are allowed.
                Must be placed after the other parameters.

";

print $USAGE;


$nargs = $#ARGV;
$nargs++;        
print "Number of args = $nargs\n";


for ($i=0; $i<$nargs; $i=$i+2) {
  if    ($ARGV[$i] eq "-o") {  
    $output = $ARGV[$i+1] ;
    print ("Output Folder: $output\n") ;
  }
  else {
    last;
  }
}

if (!$output){
  $output = "src/cloud";
}

$firstInputFile = $i;

$i = 0;

for ($i=$firstInputFile; $i<$nargs; $i++) {
    $fullname = $ARGV[$i];

    #lixo();

    #exit(0);

    print "====================\n";
    print "$ARGV[$i]\n";
    print "i = $i\n";
    print "nargs = $nargs\n";
    ($a, $b, $c) = fileparse($fullname, ".cl");
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


    ($comment, $semantics, $type, $variable, $default, $uniform) = ProcessClFile($ARGV[$i]);

    PrintCppFile($basename, $comment, $semantics, $type, $variable, $default, $is_array, $size, $output);

}



