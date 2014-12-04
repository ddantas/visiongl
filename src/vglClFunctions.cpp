#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"
#include "vglClFunctions.h"
#include <math.h>

#include <fstream>

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : require

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

  int* hist = vglClSumPartialHistogram(partial_hist,img_input->shape[VGL_WIDTH], img_input->nChannels);

  cl_int err = clReleaseMemObject( partial_hist );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  return hist;
}

int* vglClSumPartialHistogram(cl_mem partial_hist, int size, int nchannels)
{
  cl_int err;

  cl_mem mobj_histogram = NULL;
  mobj_histogram = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, nchannels*256*sizeof(int), NULL, &err);
  vglClCheckError(err, (char*) "clCreateBuffer mobj_histogram" );

  float l = log10(size)/log10(2);
  int nsize;

  if (l - floor(log10(size)/log10(2)) > 0)
    nsize = l+1;
  else
    nsize = l;

  nsize = pow(2,nsize);

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

  err = clSetKernelArg( kernel, 3, sizeof( int ), &nchannels );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  size_t worksize[] = { 256, size, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  
  int* histogram = (int*) malloc(nchannels*256*sizeof(int));
  err = clEnqueueReadBuffer(cl.commandQueue,mobj_histogram,CL_TRUE, 0, nchannels*256*sizeof(int), histogram, 0, NULL, NULL);
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
  mobj_histogram = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, img_input->nChannels*256*img_input->shape[VGL_WIDTH]*sizeof(unsigned int), NULL, &err);
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

  err = clSetKernelArg( kernel, 2, sizeof( int ), &img_input->nChannels );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

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
  mobj_histogram = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, img_input->nChannels*256*img_input->shape[VGL_WIDTH]*sizeof(unsigned int), NULL, &err);
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

    err = clSetKernelArg( kernel, 2, sizeof( int ), &img_input->nChannels );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

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

int* vglClCumulativeSum(int* arr, int size)
{
  cl_int err;
  int nsize = floor(log10(size)/log10(2));
  nsize = pow(2,nsize+1);
  cl_mem mobj_arr = NULL;
  mobj_arr = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, size*sizeof(int), NULL, &err);
  vglClCheckError(err, (char*) "clCreateBuffer mobj_arr" );
  err = clEnqueueWriteBuffer(cl.commandQueue,mobj_arr,CL_TRUE, 0, size*sizeof(int), arr, 0, NULL, NULL);
  vglClCheckError(err, (char*) "clEnqueueWriteBuffer mobj_arr");

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClMath.cl";
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
    kernel = clCreateKernel( program, "vglClCumSum", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &mobj_arr);
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( int ), &size);
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  size_t worksize[] = { nsize/2, 1, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 1, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  
  int* cumsum = (int*) malloc(size*sizeof(int));
  err = clEnqueueReadBuffer(cl.commandQueue,mobj_arr,CL_TRUE, 0, size*sizeof(int), cumsum, 0, NULL, NULL);
  vglClCheckError(err,"ReadBuffer histogram");

  err = clReleaseMemObject( mobj_arr );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_arr");

  return cumsum;
}

int* vglClCumulativeSumNorm(int* arr, int size, int norm_total)
{
  cl_int err;
  int nsize = floor(log10(size)/log10(2));
  nsize = pow(2,nsize+1);
  cl_mem mobj_arr = NULL;
  mobj_arr = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, size*sizeof(int), NULL, &err);
  vglClCheckError(err, (char*) "clCreateBuffer mobj_arr" );
  err = clEnqueueWriteBuffer(cl.commandQueue,mobj_arr,CL_TRUE, 0, size*sizeof(int), arr, 0, NULL, NULL);
  vglClCheckError(err, (char*) "clEnqueueWriteBuffer mobj_arr");

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClMath.cl";
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
    kernel = clCreateKernel( program, "vglClCumSumNorm", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &mobj_arr);
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( int ), &size);
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( int ), &norm_total);
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  size_t worksize[] = { nsize/2, 1, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 1, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  
  int* cumsum = (int*) malloc(size*sizeof(int));
  err = clEnqueueReadBuffer(cl.commandQueue,mobj_arr,CL_TRUE, 0, size*sizeof(int), cumsum, 0, NULL, NULL);
  vglClCheckError(err,"ReadBuffer histogram");

  err = clReleaseMemObject( mobj_arr );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_arr");

  return cumsum;
}

void vglClHistogramEq(VglImage* input, VglImage* output)
{
  int* hist = vglClHistogram(input);
  int* cumsum = vglClCumulativeSumNorm(hist,input->shape[VGL_WIDTH],input->shape[VGL_WIDTH]*input->shape[VGL_HEIGHT]);

  vglClGrayLevelTransform(input,output,cumsum);

}

void vglCl3dHistogramEq(VglImage* input, VglImage* output)
{
  int* hist = vglClHistogram(input);

  int* cumsum = vglClCumulativeSumNorm(hist,input->shape[VGL_WIDTH],input->shape[VGL_WIDTH]*input->shape[VGL_HEIGHT]*input->shape[VGL_LENGTH]);

  vglCl3dGrayLevelTransform(input,output,cumsum);
}

void vglClGrayLevelTransform(VglImage* input, VglImage* output, int* transformation)
{
  
  if (input->nChannels > 1 || output->nChannels > 1)
  {
     fprintf(stderr, "%s: %s: Error: image with more then 1 channel not supported. Please convert to 1 channel.\n", __FILE__, __FUNCTION__);
     return;
  }

  vglCheckContext(input,VGL_CL_CONTEXT);
  vglCheckContext(output,VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_arr = NULL;
  mobj_arr = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, 256*sizeof(int), NULL, &err);
  vglClCheckError(err, (char*) "clCreateBuffer mobj_arr" );
  err = clEnqueueWriteBuffer(cl.commandQueue,mobj_arr,CL_TRUE, 0, 256*sizeof(int), transformation, 0, NULL, NULL);
  vglClCheckError(err, (char*) "clEnqueueWriteBuffer mobj_arr");

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClHistogramEq.cl";
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
    kernel = clCreateKernel( program, "vglClGrayLevelTransform", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &input->oclPtr);
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &output->oclPtr);
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (void*) &mobj_arr);
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  size_t worksize[] = { input->shape[VGL_WIDTH], input->shape[VGL_HEIGHT], 1 };

  err = clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_arr );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_arr");

  
}

void vglCl3dGrayLevelTransform(VglImage* input, VglImage* output, int* transformation)
{
  
  if (input->nChannels > 1 || output->nChannels > 1)
  {
     fprintf(stderr, "%s: %s: Error: image with more then 1 channel not supported. Please convert to 1 channel.\n", __FILE__, __FUNCTION__);
     return;
  }

  vglCheckContext(input,VGL_CL_CONTEXT);
  vglCheckContext(output,VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_arr = NULL;
  mobj_arr = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, 256*sizeof(int), NULL, &err);
  vglClCheckError(err, (char*) "clCreateBuffer mobj_arr" );
  err = clEnqueueWriteBuffer(cl.commandQueue,mobj_arr,CL_TRUE, 0, 256*sizeof(int), transformation, 0, NULL, NULL);
  vglClCheckError(err, (char*) "clEnqueueWriteBuffer mobj_arr");

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClHistogramEq.cl";
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
    kernel = clCreateKernel( program, "vglCl3dGrayLevelTransform", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &input->oclPtr);
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &output->oclPtr);
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (void*) &mobj_arr);
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  size_t worksize[] = { input->shape[VGL_WIDTH], input->shape[VGL_HEIGHT], input->shape[VGL_LENGTH] };

  err = clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_arr );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_arr");

  
}

