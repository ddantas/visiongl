#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"


#include <fstream>

extern VglClContext cl;

void vglCl3dMergeZByMean(VglImage* img_input, VglImage* img_output, int number_of_merges){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int err;

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglCl3dMergeZByMean.cl";
    printf("Compiling %s\n", file_path);
    std::ifstream file(file_path);
    if(file.fail())
    {
      std::string str("File not found: ");
      str.append(file_path);
      vglClCheckError(-1, (char*)str.c_str());
    }
    std::string prog( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
    const char *source_str = prog.c_str();
#ifdef __DEBUG__
    printf("Kernel to be compiled:\n%s\n", source_str);
#endif
    program = clCreateProgramWithSource(cl.context, 1, (const char **) &source_str, 0, &err );
    vglClCheckError(err, (char*) "clCreateProgramWithSource" );
    err = clBuildProgram(program, 1, cl.deviceId, NULL, NULL, NULL );
    vglClBuildDebug(err, program);
  }

  static cl_kernel kernel = NULL;
  if (kernel == NULL)
  {
    kernel = clCreateKernel( program, "vglCl3dMergeZByMean", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( int ), (void*) &number_of_merges );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  if (img_input->ndim <= 2){
    printf("2D images not supported by this operation\n");
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->getWidth(), img_input->getHeight(), img_input->getLength() };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


void vglCl3dMergeZByMax(VglImage* img_input, VglImage* img_output, int number_of_merges){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int err;

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglCl3dMergeZByMax.cl";
    printf("Compiling %s\n", file_path);
    std::ifstream file(file_path);
    if(file.fail())
    {
      std::string str("File not found: ");
      str.append(file_path);
      vglClCheckError(-1, (char*)str.c_str());
    }
    std::string prog( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
    const char *source_str = prog.c_str();
#ifdef __DEBUG__
    printf("Kernel to be compiled:\n%s\n", source_str);
#endif
    program = clCreateProgramWithSource(cl.context, 1, (const char **) &source_str, 0, &err );
    vglClCheckError(err, (char*) "clCreateProgramWithSource" );
    err = clBuildProgram(program, 1, cl.deviceId, NULL, NULL, NULL );
    vglClBuildDebug(err, program);
  }

  static cl_kernel kernel = NULL;
  if (kernel == NULL)
  {
    kernel = clCreateKernel( program, "vglCl3dMergeZByMax", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( int ), (void*) &number_of_merges );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  if (img_input->ndim <= 2){
    printf("2D images not supported by this operation\n");
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->getWidth(), img_input->getHeight(), img_input->getLength() };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

float vglCl9root(float number)
{

	cl_int err;

	static cl_program program = NULL;
	if (program == NULL)
	{
		char* file_path = (char*) "CL/roottest.cl";
		printf("Compiling %s\n", file_path);
		std::ifstream file(file_path);
		if(file.fail())
		{
			std::string str("File not found: ");
			str.append(file_path);
			vglClCheckError(-1, (char*)str.c_str());
		}
		std::string prog( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
		const char *source_str = prog.c_str();
#ifdef __DEBUG__
		printf("Kernel to be compiled:\n%s\n", source_str);
#endif
		program = clCreateProgramWithSource(cl.context, 1, (const char **) &source_str, 0, &err );
		vglClCheckError(err, (char*) "clCreateProgramWithSource" );
		err = clBuildProgram(program, 1, cl.deviceId, NULL, NULL, NULL );
		vglClBuildDebug(err, program);
	}

	static cl_kernel kernel = NULL;
	if (kernel == NULL)
	{
	    kernel = clCreateKernel( program, "vglCl9Root", &err ); 
	    vglClCheckError(err, (char*) "clCreateKernel" );
	}
	
	cl_mem input = clCreateBuffer(cl.context,CL_MEM_READ_ONLY,sizeof(float),NULL,&err);
	clEnqueueWriteBuffer(cl.commandQueue,input,true,0,sizeof(float),&number,0,0,0);

	cl_mem output = clCreateBuffer(cl.context,CL_MEM_READ_ONLY,sizeof(float),NULL,&err);
	
	err = clSetKernelArg( kernel, 0, sizeof(cl_mem ), &input );
	vglClCheckError( err, (char*) "clSetKernelArg 0" );
	
	err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), &output );
	vglClCheckError( err, (char*) "clSetKernelArg 1" );
	
	size_t worksize[] = { 1, 1, 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 1, NULL, worksize, 0, 0, 0, 0 );

	
	float ret;
	clEnqueueReadBuffer(cl.commandQueue,output,true,0,sizeof(float),&ret,0,0,0);
	

	clReleaseMemObject(input);
	clReleaseMemObject(output);

	return ret;
}
