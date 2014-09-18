
#define _CRT_SECURE_NO_WARNINGS

#include <visiongl.h>

#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>

#include <fstream>
#include <string>

#define NOT_ENOUGH_ARGS_MSG "\nNot enough arguments provided.\n"
#define WRONG_USAGE "\nWrong usage, you must first add an argument for execution.\n"
#define LACKING_VALUE_FOR_ARG "\nYou passed wrong value for %s argument\n"
#define WRONG_TYPE_TO_ARG "\nYou've put a wrong value type in an argument\n"
#define MISSING_SETUP_ARGS_BEFORE "\nYou forgot to set some arguments before calling this argument: %s\n"

using namespace std;

string input;
int isset_input = 0;

string output;
int isset_output = 0;

int window_size_x;
int isset_window_size_x = 0;

int window_size_y;
int isset_window_size_y = 0;

float* convolution_window;
int isset_convolution_window = 0;

string* getValue(int arg_position, int argc, char* argv[])
{
    if (arg_position+1 > argc || strcmp(string(argv[arg_position+1]).substr(0,2).c_str(),"--") == 0)
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
void convertStringToArray(string array, float* convolution_window, int size)
{
    array = array.substr(1,array.length()-2);
    array = replaceinString(array," ","");

    char* value = strtok((char*)array.c_str(),",");
    for (int i = 0; i < size; i++)
    {
        float f;
        if(sscanf(value, "%f", &f)  == -1 )
        {
            printf(WRONG_TYPE_TO_ARG);
            printf("argument: window_convolution\n");
            printf("erro reading vector position %d\n",i+1);            
            exit(1);
        }
        convolution_window[i] = f;
        value = strtok(NULL,",");
    }
}

void processArgs(int argc, char* argv[])
{
    if (argc > 1)
    {
        for(int i = 1; i < argc; i++)
        {
            string arg = string(argv[i]);
            //primeira palavra após o nome do executável deve ser um
            //argumento e não um valor
            if (i == 1 && strcmp(arg.substr(0,2).c_str(),"--") != 0)
            {
                printf(WRONG_USAGE);
                exit(1);
            }
            else
            {
                if (strcmp(arg.c_str(),"--input") == 0)
                {
                    string value = * getValue(i,argc,argv);
                    input = value;
                    isset_input = 1;
                }
                else if (strcmp(arg.c_str(),"--output") == 0)
                {
                    string value = * getValue(i,argc,argv);
                    output = value;
                    isset_output = 1;
                }
                else if(strcmp(arg.c_str(),"--window_size_x") == 0)
                {
                    string value = * getValue(i,argc,argv);
                    int ivalue;
                    if(sscanf(value.c_str(), "%d", &ivalue)  == -1 )
                    {
                        printf(WRONG_TYPE_TO_ARG);
                        exit(1);
                    }
                    window_size_x = ivalue;
                    isset_window_size_x = 1;
                }
                else if(strcmp(arg.c_str(),"--window_size_y") == 0)
                {
                    string value = * getValue(i,argc,argv);
                    int ivalue;
                    if(sscanf(value.c_str(), "%d", &ivalue)  == -1 )
                    {
                        printf(WRONG_TYPE_TO_ARG);
                        exit(1);
                    }
                    window_size_y = ivalue;
                    isset_window_size_x = 1;
                }
                else if(strcmp(arg.c_str(),"--window_convolution") == 0)
                {
                    if (window_size_x == -1 || window_size_y == -1)
                    {
		        //printf(MISSING_SETUP_ARGS_BEFORE,"--window_convolution");                        
                    }
                    else
		    {
                        string value = *getValue(i,argc,argv);
                        convolution_window = (float*)malloc(sizeof(float)*window_size_x*window_size_y);
                        convertStringToArray(value,convolution_window,window_size_x,window_size_y);
                        isset_window_convolution = 1;
                    }
                }

            }
        }
    }
    else
    {
        fprintf(stderr, NOT_ENOUGH_ARGS_MSG);
    }
}

int main(int argc, char* argv[])
{
    processArgs(argc, argv);
    processArgs(argc, argv);

    printf("input: %s\n", input.c_str());
    printf("output: %s\n", output.c_str());
    printf("window_size_x: %d\n", window_size_x);
    printf("window_size_y: %d\n", window_size_y);

    printf("convolution_window: [");
    for (int i = 0; i < window_size_x * window_size_y; i++)
    {
      if (i > 0) printf(", ");
      printf("%f", convolution_window[i]);
    }
    printf("]\n");
    
    if (!isset_input)
    {
        printf("Missing input image path, try using --input path/image.ext \n");
        exit(1);
    }
    if (!isset_output)
    {
        printf("Missing output image path, try using --output path/image.ext \n");
        exit(1);
    }
    if (!isset_convolution_window)
    {
        printf("You forgot to set convolution_window structuring element, try using --window_convolution [1,2,3,...,n] and remember to set --window_size_x and --window_size_y before using --window_convolution");
        exit(1);
    }

    vglInit(30,30);
    vglClInit();

    VglImage* img_input = vglLoadImage((char*) input.c_str(),1);
    VglImage* img_output = NULL;

    if (img_input->nChannels == 3)
    {
        vglImage3To4Channels(img_input);
    }

    img_output = vglCreateImage(img_input);
    vglClConvolution(img_input, img_output, convolution_window, window_size_x, window_size_y);


    vglCheckContext(img_output, VGL_RAM_CONTEXT);
    cvSaveImage(output.c_str(), img_output->ipl);

    return 0;
}
