#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"
#include "vglClFunctions.h"
#include "cl2cpp_shaders.h"
#include "cl2cpp_MM.h"
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

  size_t worksize[] = { 256, size, nchannels };
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

void vglCl3dErode(VglImage* input, VglImage* output, VglImage* buff, float* mask, int window_size_x, int window_size_y, int window_size_z, int times)
{

  if(input->ndim < 3)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglClErode instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  if (times > 0)
  {

    vglCl3dErode(input,buff,mask,window_size_x,window_size_y,window_size_z);
  
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglCl3dErode(output,buff,mask,window_size_x,window_size_y,window_size_z);
      else
        vglCl3dErode(buff,output,mask,window_size_x,window_size_y,window_size_z);
    }

    if (times % 2 == 1)
      vglCl3dCopy(buff,output);
  }

}

void vglClErode(VglImage* input, VglImage* output, VglImage* buff, float* mask, int window_size_x, int window_size_y, int times)
{

  if(input->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with more then 2 dimensions not supported. Use vglCl3dErode instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  if (times > 0)
  {
    vglClErode(input,buff,mask,window_size_x,window_size_y);
  
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglClErode(output,buff,mask,window_size_x,window_size_y);
      else
        vglClErode(buff,output,mask,window_size_x,window_size_y);
    }
  
    if (times % 2 == 1)
      output = buff;
  }
}

void vglCl3dDilate(VglImage* input, VglImage* output, VglImage* buff, float* mask, int window_size_x, int window_size_y,int window_size_z, int times)
{

  if(input->ndim < 3)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglClDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  if (times > 0)
  {
  
    vglCl3dDilate(input,buff,mask,window_size_x,window_size_y,window_size_z);
  
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglCl3dDilate(output,buff,mask,window_size_x,window_size_y,window_size_z);
      else
        vglCl3dDilate(buff,output,mask,window_size_x,window_size_y,window_size_z);
    }

    if (times % 2 == 1)
      output = buff;
  }
}

void vglClDilate(VglImage* input, VglImage* output, VglImage* buff, float* mask, int window_size_x, int window_size_y, int times)
{

  if(input->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with more then 2 dimensions not supported. Use vglCl3dDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  if (times > 0)
  {

    vglClDilate(input,buff,mask,window_size_x,window_size_y);
  
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglClDilate(output,buff,mask,window_size_x,window_size_y);
      else
        vglClDilate(buff,output,mask,window_size_x,window_size_y);
    }

    if (times % 2 == 1)
      output = buff;
  }
}

void vglCl3dDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times){

  if(src->ndim < 3)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglClDistTransform instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglCl3dThreshold(src, buf, 0.0, 1.0f/256.0f);
  vglCl3dCopy(buf, dst);
  float square_mask[27] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float cross_mask[27] = {0,0,0,
                          0,1,0,
                          0,0,0,
                          0,1,0,
                          1,1,1,
                          0,1,0,
                          0,0,0,
                          0,1,0,
                          0,0,0};

  float mask1[3] = {0,1,1};
  float mask2[3] = {1,1,0};
  for(int i = 0; i < times; i++){
    if (i % 6 == 0){
      vglCl3dFuzzyHamacherErode(buf, buf2,mask1,3,1,1,0.2f);
      vglCl3dSum(buf2, dst, dst);
    }
    else if (i % 6 == 1){
      vglCl3dFuzzyHamacherErode(buf2, buf,mask2,3,1,1,0.2f);
      vglCl3dSum(buf, dst, dst);
    }
    else if (i % 6 == 2){
      vglCl3dFuzzyHamacherErode(buf, buf2,mask1,1,3,1,0.2f);
      vglCl3dSum(buf2, dst, dst);
    }
    else if (i % 6 == 3){
      vglCl3dFuzzyHamacherErode(buf2, buf,mask2,1,3,1,0.2f);
      vglCl3dSum(buf, dst, dst);
    }
    else if (i % 6 == 4){
      vglCl3dFuzzyHamacherErode(buf, buf2,mask1,1,1,3,0.2f);
      vglCl3dSum(buf2, dst, dst);
    }
    else{
      vglCl3dFuzzyHamacherErode(buf2, buf,mask2,1,1,3,0.2f);
      vglCl3dSum(buf, dst, dst);
    }
  }
}

void vglClDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times){
  
  if(src->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with more then 2 dimensions not supported. Use vglCl3dDistTransform instead.\n", __FILE__, __FUNCTION__);
    return;
  }
  
  vglClThreshold(src, buf, 0.0, 1.0f/256.0);
  vglClCopy(buf, dst);
  float square_mask[9] = {1,1,1,1,1,1,1,1,1};
  float cross_mask[9] = {0,1,0,1,1,1,0,1,0};
  for(int i = 0; i < times; i++){
    if (i % 2 == 0){
      vglClErode(buf, buf2,square_mask,3,3);
      vglClSum(buf2, dst, dst);
    }
    else{
      vglClErode(buf2, buf,cross_mask,3,3);
      vglClSum(buf, dst, dst);
    }
  }
}

void vglClTopHat(VglImage* src, VglImage* dst, VglImage* buf, float* window, int window_size_x, int window_size_y, int times)
{

  vglClErode(src,dst,buf,window,window_size_x,window_size_y,times);
  vglClDilate(src,dst,buf,window,window_size_x,window_size_y,times);

  //must implement vglClSub
  //vglClSub(src,dst,dst);
}

void vglCl3dTopHat(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, float* window, int window_size_x, int window_size_y, int window_size_z, int times)
{

  vglCl3dErode(src,dst,buf,window,window_size_x,window_size_y,window_size_z,times);
  vglCl3dDilate(dst,buf,buf2,window,window_size_x,window_size_y,window_size_z,times);

  vglCl3dSub(src,buf,dst);
}

bool vglCl3dEqual(VglImage* input1, VglImage* input2)
{
  if(input1->ndim < 3)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglClEqual instead.\n", __FILE__, __FUNCTION__);
    return false;
  }

  vglCheckContext(input1,VGL_CL_CONTEXT);
  vglCheckContext(input2,VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_equal = NULL;
  mobj_equal = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, sizeof(char), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer histogram" );
  unsigned char e = 200;
  err = clEnqueueWriteBuffer(cl.commandQueue,mobj_equal,CL_TRUE,0,sizeof(char),&e,0,NULL,NULL);
  vglClCheckError(err, (char*) "clEnqueueWriteBuffer mobj_arr");

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClEqual.cl";
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
    kernel = clCreateKernel( program, "vglCl3dEqual", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &input1->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &input2->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (void*) &mobj_equal );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  size_t worksize[] = { input1->shape[VGL_WIDTH], input1->shape[VGL_HEIGHT], input1->shape[VGL_LENGTH] };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  
  err = clEnqueueReadBuffer(cl.commandQueue,mobj_equal,CL_TRUE,0,sizeof(char),&e,0,NULL,NULL);
  vglClCheckError( err, (char*) "clEnqueueReadBuffer" );
  
  return e != 1;
}

bool vglClEqual(VglImage* input1, VglImage* input2)
{
  if(input1->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with more then 2 dimensions not supported. Use vglCl3dEqual instead.\n", __FILE__, __FUNCTION__);
    return false;
  }

  vglCheckContext(input1,VGL_CL_CONTEXT);
  vglCheckContext(input2,VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_equal = NULL;
  mobj_equal = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, sizeof(char), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer histogram" );
  char e = 200;
  err = clEnqueueWriteBuffer(cl.commandQueue,mobj_equal,CL_TRUE,0,sizeof(char),&e,0,NULL,NULL);
  vglClCheckError(err, (char*) "clEnqueueWriteBuffer mobj_arr");

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL_UTIL/vglClEqual.cl";
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
    kernel = clCreateKernel( program, "vglCl3dEqual", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &input1->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &input2->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (void*) &mobj_equal );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  size_t worksize[] = { input1->shape[VGL_WIDTH], input1->shape[VGL_HEIGHT], 0 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  
  err = clEnqueueReadBuffer(cl.commandQueue,mobj_equal,CL_TRUE,0,sizeof(char),&e,0,NULL,NULL);
  vglClCheckError( err, (char*) "clEnqueueReadBuffer" );

  return e != 1;
}

void vglCl3dConditionalDilate(VglImage* src, VglImage* dst, VglImage* mask, float* window, int window_size_x, int window_size_y, int window_size_z)
{

  if(src->ndim < 3)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglClConditionalDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglCl3dDilate(src,dst,window,window_size_x,window_size_y,window_size_z);
  vglCl3dMin(mask,dst,dst);
  
}

void vglClConditionalDilate(VglImage* src, VglImage* dst, VglImage* mask, float* window, int window_size_x, int window_size_y)
{

  if(src->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglCl3dConditionalDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglClDilate(src,dst,window,window_size_x,window_size_y);
  vglClMin(mask,dst,dst);
  
}

void vglCl3dConditionalErode(VglImage* src, VglImage* dst, VglImage* mask,  float* window, int window_size_x, int window_size_y, int window_size_z)
{

  if(src->ndim < 3)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglClConditionalErode instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglCl3dErode(src,dst,window,window_size_x,window_size_y,window_size_z);
  vglCl3dMax(mask,dst,dst);
  
}

void vglClConditionalErode(VglImage* src, VglImage* dst, VglImage* mask,  float* window, int window_size_x, int window_size_y)
{

  if(src->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglCl3dConditionalErode instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglClErode(src,dst,window,window_size_x,window_size_y);
  vglClMax(mask,dst,dst);
}

void vglCl3dConditionalDilate(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, float* window, int window_size_x, int window_size_y, int window_size_z, int times)
{
    if (times > 0)
    {
      vglCl3dConditionalDilate(src,buff,mask,window,window_size_x,window_size_y,window_size_z);

      for(int i = 1; i < times; i++)
      {
        if (i % 2 == 0)
          vglCl3dConditionalDilate(dst,buff,mask,window,window_size_x,window_size_y,window_size_z);
        else
          vglCl3dConditionalDilate(buff,dst,mask,window,window_size_x,window_size_y,window_size_z);
      }

      if (times % 2 == 1)
        vglCl3dCopy(buff,dst);
    }
}

void vglClConditionalDilate(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, float* window, int window_size_x, int window_size_y, int times)
{
    if (times > 0)
    {
      vglClConditionalDilate(src,buff,mask,window,window_size_x,window_size_y);

      for(int i = 1; i < times; i++)
      {
        if (i % 2 == 0)
           vglClConditionalDilate(dst,buff,mask,window,window_size_x,window_size_y);
        else
          vglClConditionalDilate(buff,dst,mask,window,window_size_x,window_size_y);
      }

      if (times % 2 == 1)
        vglCl3dCopy(buff,dst);
    }
}

void vglCl3dConditionalErode(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, float* window, int window_size_x, int window_size_y, int window_size_z, int times)
{
    if (times > 0)
    {
      vglCl3dConditionalErode(src,buff,mask,window,window_size_x,window_size_y,window_size_z);

      for(int i = 1; i < times; i++)
      {
        if (i % 2 == 0)
           vglCl3dConditionalErode(dst,buff,mask,window,window_size_x,window_size_y,window_size_z);
        else
          vglCl3dConditionalErode(buff,dst,mask,window,window_size_x,window_size_y,window_size_z);
      }

      if (times % 2 == 1)
        vglCl3dCopy(buff,dst);
    }
}

void vglClConditionalErode(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, float* window, int window_size_x, int window_size_y, int times)
{
    if (times > 0)
    {
      vglClConditionalErode(src,buff,mask,window,window_size_x,window_size_y);

      for(int i = 1; i < times; i++)
      {
        if (i % 2 == 0)
           vglClConditionalErode(dst,buff,mask,window,window_size_x,window_size_y);
        else
          vglClConditionalErode(buff,dst,mask,window,window_size_x,window_size_y);
      }

      if (times % 2 == 1)
        vglCl3dCopy(buff,dst);
    }
}


/*src = X
  mask = Y
  dst = buffer1, também é a vaiavel de saida
  buff = buffer2,
  buff2 = buffer3
  em dst vai ficar a n-esima erosao condicional
  em buff vai ficar a passo anterior do somatório das uniões para comparação
  em buff2 vai ficar o passo atual do somatório das uniões para comparação de conergencia com buff*/
void vglCl3dDilateFromMarker(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, VglImage* buff2, float* window, int window_size_x, int window_size_y,int window_size_z)
{

  vglCl3dConditionalDilate(mask,dst,src,window,window_size_x,window_size_y,window_size_z);
  vglCl3dCopy(dst,buff2);

  int c = 0;
  while(!vglCl3dEqual(buff,buff2))
  {
    if (c % 2 == 0)
    {
      vglCl3dConditionalDilate(dst,mask,src,window,window_size_x,window_size_y,window_size_z);
      vglCl3dCopy(buff2,buff);
      vglCl3dMax(mask,buff2,buff2);
      //vglCl3dCopy(mask,dst);
    }
    else
    {
      vglCl3dConditionalDilate(mask,dst,src,window,window_size_x,window_size_y,window_size_z);
      vglCl3dCopy(buff2,buff);
      vglCl3dMax(dst,buff2,buff2);
    }
    c++;
  }

  vglCl3dCopy(buff2,dst);

}

void vglClDilateFromMarker(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, VglImage* buff2, float* window, int window_size_x, int window_size_y)
{

  vglClConditionalDilate(mask,dst,src,window,window_size_x,window_size_y);
  vglClCopy(dst,buff2);

  int c = 0;
  while(!vglClEqual(buff,buff2))
  {
    if (c % 2 == 0)
    {
      vglClConditionalDilate(dst,mask,src,window,window_size_x,window_size_y);
      vglClCopy(buff2,buff);
      vglClMax(mask,buff2,buff2);
      //vglCl3dCopy(mask,dst);
    }
    else
    {
      vglClConditionalDilate(mask,dst,src,window,window_size_x,window_size_y);
      vglClCopy(buff2,buff);
      vglClMax(dst,buff2,buff2);
    }
    c++;
  }

  vglCl3dCopy(buff2,dst);

}

void vglCl3dErodeFromMarker(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, VglImage* buff2, float* window, int window_size_x, int window_size_y,int window_size_z)
{

  vglCl3dConditionalErode(mask,dst,src,window,window_size_x,window_size_y,window_size_z);
  vglCl3dCopy(dst,buff2);

  int c = 0;
  while(!vglCl3dEqual(buff,buff2))
  {
    if (c % 2 == 0)
    {
      vglCl3dConditionalErode(dst,mask,src,window,window_size_x,window_size_y,window_size_z);
      vglCl3dCopy(buff2,buff);
      vglCl3dMin(mask,buff2,buff2);
      //vglCl3dCopy(mask,dst);
    }
    else
    {
      vglCl3dConditionalErode(mask,dst,src,window,window_size_x,window_size_y,window_size_z);
      vglCl3dCopy(buff2,buff);
      vglCl3dMin(dst,buff2,buff2);
    }
    c++;
  }

  vglCl3dCopy(buff2,dst);

}

void vglClErodeFromMarker(VglImage* src, VglImage* dst, VglImage* mask, VglImage* buff, VglImage* buff2, float* window, int window_size_x, int window_size_y)
{

  vglClConditionalErode(mask,dst,src,window,window_size_x,window_size_y);
  vglClCopy(dst,buff2);

  int c = 0;
  while(!vglClEqual(buff,buff2))
  {
    if (c % 2 == 0)
    {
      vglClConditionalErode(dst,mask,src,window,window_size_x,window_size_y);
      vglClCopy(buff2,buff);
      vglClMin(mask,buff2,buff2);
      //vglCl3dCopy(mask,dst);
    }
    else
    {
      vglClConditionalErode(mask,dst,src,window,window_size_x,window_size_y);
      vglClCopy(buff2,buff);
      vglClMin(dst,buff2,buff2);
    }
    c++;
  }

  vglCl3dCopy(buff2,dst);

}

void vglCl3dLastErosion(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglImage* buff3, VglImage* buff4, VglImage* buff5, float* window, int window_size_x, int window_size_y,int window_size_z)
{
  
  vglCl3dErode(src, dst, window, window_size_x, window_size_y, window_size_z);
  vglCl3dErode(dst, buff, window, window_size_x, window_size_y, window_size_z);
  vglCl3dDilateFromMarker(dst,buff, buff, buff2, buff3, window, window_size_x, window_size_y, window_size_z);  
  vglCl3dNot(buff,buff);
  vglCl3dMin(dst,buff,buff);
  vglCl3dCopy(buff,buff5);
  
  int c = 0;
  while(!vglCl3dEqual(buff4,buff5))
  {
      vglCl3dErode(dst,buff,window,window_size_x, window_size_y, window_size_z);
      vglCl3dErode(buff,buff4,window,window_size_x, window_size_y, window_size_z);
      vglCl3dDilateFromMarker(buff,buff4,buff4,buff2,buff3,window,window_size_x, window_size_y, window_size_z);
      vglCl3dNot(buff4,buff3);
      vglCl3dCopy(buff5,buff4);
      vglClMin(buff,buff3,buff2);
      vglClMin(buff4,buff5,buff5);
      vglCl3dCopy(buff,dst);
      c++;
  }
  
  vglCl3dCopy(buff5,dst);
}