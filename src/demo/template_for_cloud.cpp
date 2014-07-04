
#define _CRT_SECURE_NO_WARNINGS
#define __OPENCL__
#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include <fstream>
#include <string>

/*
	argv[1] = input_image_path
	argv[2] = number of operations to execute
*/

#define NOT_ENOUGH_ARGS_MSG "\nNot enough arguments provided.\n"
#define WRONG_USAGE "\nWrong usage, you must first add an argument for execution.\n"
#define LACKING_VALUE_FOR_ARG "\nYou passed wrong value for %s argument\n"
#define WRONG_TYPE_TO_ARG "\nYou've put a wrong value type in an argument\n"
#define MISSING_SETUP_ARGS_BEFORE "\nYou forgot to set some arguments before calling this argument: %s\n"

using namespace std;

string input_path;
string output_path;
int window_size_x = -1, window_size_y = -1;
float* convolution_window;

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
void process_string_to_array(string array,float* convolution_window,int size_x, int size_y)
{
	array = array.substr(1,array.length()-2);
	array = replaceinString(array," ","");

	char* value = strtok((char*)array.c_str(),",");
	for (int i = 0; i < size_x*size_y; i++)
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

void process_args(int argc, char* argv[])
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
					input_path = value;
				}
				else if (strcmp(arg.c_str(),"--output") == 0)
				{
					string value = * getValue(i,argc,argv);
					output_path = value;
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
				}
				else if(strcmp(arg.c_str(),"--window_convolution") == 0)
				{
					if (window_size_x == -1 || window_size_y == -1)
					{
						printf(MISSING_SETUP_ARGS_BEFORE,"--window_convolution");
						exit(1);
					}
					string value = *getValue(i,argc,argv);
					convolution_window = (float*)malloc(sizeof(float)*window_size_x*window_size_y);
					process_string_to_array(value,convolution_window,window_size_x,window_size_y);
				}

			}
		}
	}
	else
	{
		printf(NOT_ENOUGH_ARGS_MSG);
	}

}

int main(int argc, char* argv[])
{
	process_args(argc,argv);
    
	if (input_path.empty())
	{
		printf("Missing input image path, try using --input path/image.ext \n");
		exit(1);
	}
	if (output_path.empty())
	{
		printf("Missing output image path, try using --output path/image.ext \n");
		exit(1);
	}

	if (convolution_window == NULL)
	{
		printf("You forgot to set convolution_window structuring element, try using --window_convolution [1,2,3,...,n] and remember to set --window_size_x and --window_size_y before using --window_convolution");
		exit(1);
	}

	vglInit(30,30);
	vglClInit();

	VglImage* input = vglLoadImage((char*) input_path.c_str(),1);
	VglImage* output = vglCreateImage(input);
	
	vglClConvolution(input,output,convolution_window,window_size_x,window_size_y);

	vglCheckContext(output, VGL_RAM_CONTEXT);
	cvSaveImage(output_path.c_str(), output->ipl);

	return 0;
}
