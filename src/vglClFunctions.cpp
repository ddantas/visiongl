
#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"
#include "vglClFunctions.h"


#include <fstream>

extern VglClContext cl;

int* vglClHistogram(VglImage* img_input){
  cl_mem partial_hist;

  if (img_input->ndim == 2)
  {
    partial_hist = vglClPartialHistogram(img_input);
  }
  else if (img_input->ndim == 3)
  {
    partial_hist = vglCl3dPartialHistogram(img_input);
  }

  int* hist = vglClSumPartialHistogram(partial_hist,img_input->shape[VGL_WIDTH]);

  cl_int err = clReleaseMemObject( partial_hist );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  return hist;
}

int* vglClSumPartialHistogram(cl_mem partial_hist, int size)
{
  cl_int err;

  cl_mem mobj_histogram = NULL;
  mobj_histogram = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, 3*256*sizeof(int), NULL, &err);
  vglClCheckError(err, (char*) "clCreateBuffer mobj_histogram" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClHistogram.cl";
    printf("Compiling %s\n", file_path);
    std::ifstream file(file_path);
    if(file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, file_path);
      exit(1);
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
    kernel = clCreateKernel( program, "vglClSumPartialHistogram", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &partial_hist);
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &mobj_histogram );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( unsigned int ), &size );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  size_t worksize[] = { 256, 1, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 1, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  
  int* histogram = (int*) malloc(3*256*sizeof(int));
  err = clEnqueueReadBuffer(cl.commandQueue,mobj_histogram,CL_TRUE, 0, 3*256*sizeof(int), histogram, 0, NULL, NULL);
  vglClCheckError(err,"ReadBuffer histogram");

  err = clReleaseMemObject( mobj_histogram );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  return histogram;
}

cl_mem vglClPartialHistogram(VglImage* img_input)
{

  vglCheckContext(img_input, VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_histogram = NULL;
  mobj_histogram = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, 3*256*img_input->shape[VGL_WIDTH]*sizeof(unsigned int), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer histogram" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClHistogram.cl";
    printf("Compiling %s\n", file_path);
    std::ifstream file(file_path);
    if(file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, file_path);
      exit(1);
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
    kernel = clCreateKernel( program, "vglCl2dPartialHistogram", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &mobj_histogram );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], 1, 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 1, NULL, worksize, 0, 0, 0, 0 );
    vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  }
  else{
    printf("More than 2 dimensions, try to use the 3d version of the function\n");
  }

  return mobj_histogram;
}

cl_mem vglCl3dPartialHistogram(VglImage* img_input)
{

  vglCheckContext(img_input, VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_histogram = NULL;
  mobj_histogram = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, 3*256*img_input->shape[VGL_WIDTH]*sizeof(unsigned int), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer histogram" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClHistogram.cl";
    printf("Compiling %s\n", file_path);
    std::ifstream file(file_path);
    if(file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, file_path);
      exit(1);
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
    kernel = clCreateKernel( program, "vglCl3dPartialHistogram", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &mobj_histogram );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], 1, 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 1, NULL, worksize, 0, 0, 0, 0 );
    vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  }
  else{
    printf("dimension different of 3 not supported by this function, try to use another version of the function\n");
  }

  return mobj_histogram;
}