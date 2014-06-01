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
#define LACKING_VALUE_FOR_ARG "\n You missed pass the value for %s argument\n"

using namespace std;

string input_path;
string output_path;

string* getValue(int arg_position, int argc, char* argv[])
{
	if (arg_position+1 > argc || strcmp(string(argv[arg_position+1]).substr(0,2).c_str(),"--") == 0)
	{
		printf(LACKING_VALUE_FOR_ARG,argv[arg_position]);
	}
	else
		return new string(argv[arg_position+1]);
}

void process_args(int argc, char* argv[])
{
	if (argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			string arg = string(argv[i]);
			if (i == 1 && strcmp(arg.substr(0,2).c_str(),"--") != 0)
			{
				printf(WRONG_USAGE);
				exit(0);
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
	
	return 0;
}
