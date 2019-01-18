#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"
#include "vglClFunctions.h"
#include "cl2cpp_shaders.h"
#include "cl2cpp_MM.h"
#include "cl2cpp_BIN.h"
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

  int* hist = vglClSumPartialHistogram(partial_hist,img_input->getWidth(), img_input->nChannels);

  cl_int err = clReleaseMemObject( partial_hist );
  vglClCheckError(err, (char*) "clReleaseMemObject partial_hist");

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
  vglClCheckError(err, (char*) "ReadBuffer histogram");

  err = clReleaseMemObject( mobj_histogram );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_histogram");

  return histogram;
}

cl_mem vglClPartialHistogram(VglImage* img_input)
{

  vglCheckContext(img_input, VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_histogram = NULL;
  mobj_histogram = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, img_input->nChannels*256*img_input->getWidth()*sizeof(unsigned int), NULL, &err);
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
    size_t worksize[] = { img_input->getWidth(), 1, 1 };
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
  mobj_histogram = clCreateBuffer(cl.context, CL_MEM_READ_WRITE, img_input->nChannels*256*img_input->getWidth()*sizeof(unsigned int), NULL, &err);
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
    size_t worksize[] = { img_input->getWidth(), 1, 1 };
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
  vglClCheckError(err, (char*) "ReadBuffer histogram");

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
  vglClCheckError(err, (char*) "ReadBuffer histogram");

  err = clReleaseMemObject( mobj_arr );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_arr");

  return cumsum;
}

void vglClHistogramEq(VglImage* input, VglImage* output)
{
  int* hist = vglClHistogram(input);
  int* cumsum = vglClCumulativeSumNorm(hist,input->getWidth(),input->getWidth()*input->getHeight());

  vglClGrayLevelTransform(input,output,cumsum);

}

void vglCl3dHistogramEq(VglImage* input, VglImage* output)
{
  int* hist = vglClHistogram(input);

  int* cumsum = vglClCumulativeSumNorm(hist,input->getWidth(),input->getWidth()*input->getHeight()*input->getLength());

  vglCl3dGrayLevelTransform(input,output,cumsum);
}

void vglClGrayLevelTransform(VglImage* input, VglImage* output, int* transformation)
{
  
  if (input->nChannels > 1 || output->nChannels > 1)
  {
     fprintf(stderr, "%s: %s: Error: image with more than 1 channel not supported. Please convert to 1 channel.\n", __FILE__, __FUNCTION__);
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

  size_t worksize[] = { input->getWidth(), input->getHeight(), 1 };

  err = clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_arr );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_arr");

  
}

void vglCl3dGrayLevelTransform(VglImage* input, VglImage* output, int* transformation)
{
  
  if (input->nChannels > 1 || output->nChannels > 1)
  {
     fprintf(stderr, "%s: %s: Error: image with more than 1 channel not supported. Please convert to 1 channel.\n", __FILE__, __FUNCTION__);
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

  size_t worksize[] = { input->getWidthIn(), input->getHeightIn(), input->getLength() };

  err = clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_arr );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_arr");

  
}

void vglCl3dErode(VglImage* input, VglImage* output, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int times)
{

  if(input->ndim < 3)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglClErode instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  if (times > 0)
  {

    vglCl3dErode(input, buff, strel, strel_size_x, strel_size_y, strel_size_z);
  
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglCl3dErode(output, buff, strel, strel_size_x, strel_size_y, strel_size_z);
      else
        vglCl3dErode(buff, output, strel, strel_size_x, strel_size_y, strel_size_z);
    }

    if (times % 2 == 1)
      vglCl3dCopy(buff, output);
  }

}

void vglClErode(VglImage* input, VglImage* output, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int times)
{

  if(input->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with more than 2 dimensions not supported. Use vglCl3dErode instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  if (times > 0)
  {
    vglClErode(input, buff, strel, strel_size_x, strel_size_y);
  
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglClErode(output, buff, strel, strel_size_x, strel_size_y);
      else
        vglClErode(buff, output, strel, strel_size_x, strel_size_y);
    }
  
    if (times % 2 == 1)
      vglCl3dCopy(buff, output);
  }
}

void vglCl3dDilate(VglImage* input, VglImage* output, VglImage* buff, float* strel, int strel_size_x, int strel_size_y,int strel_size_z, int times)
{

  if(input->ndim < 3)
  {
    fprintf(stderr, "%s: %s: Error: image with less then 3 dimensions not supported. Use vglClDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  if (times > 0)
  {
  
    vglCl3dDilate(input, buff, strel, strel_size_x, strel_size_y, strel_size_z);
  
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglCl3dDilate(output, buff, strel, strel_size_x, strel_size_y, strel_size_z);
      else
        vglCl3dDilate(buff, output, strel, strel_size_x, strel_size_y, strel_size_z);
    }

    if (times % 2 == 1)
      vglCl3dCopy(buff, output);
  }
}

void vglClDilate(VglImage* input, VglImage* output, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int times)
{

  if(input->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with more than 2 dimensions not supported. Use vglCl3dDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  if (times > 0)
  {

    vglClDilate(input, buff, strel, strel_size_x, strel_size_y);
  
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglClDilate(output, buff, strel, strel_size_x, strel_size_y);
      else
        vglClDilate(buff, output, strel, strel_size_x, strel_size_y);
    }

    if (times % 2 == 1)
      vglCl3dCopy(buff, output);
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
  float square_strel[27] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float cross_strel[27] = {0,0,0,
                          0,1,0,
                          0,0,0,
                          0,1,0,
                          1,1,1,
                          0,1,0,
                          0,0,0,
                          0,1,0,
                          0,0,0};

  float strel1[3] = {0,1,1};
  float strel2[3] = {1,1,0};
  for(int i = 0; i < times; i++){
    if (i % 6 == 0){
      vglCl3dErode(buf, buf2, strel1, 3, 1, 1);      
      vglCl3dSum(buf2, dst, dst);
    }
    else if (i % 6 == 1){
      vglCl3dErode(buf2, buf, strel2, 3, 1, 1);
      vglCl3dSum(buf, dst, dst);
    }
    else if (i % 6 == 2){
      vglCl3dErode(buf, buf2,strel1,1,3,1);
      vglCl3dSum(buf2, dst, dst);
    }
    else if (i % 6 == 3){
      vglCl3dErode(buf2, buf, strel2, 1, 3, 1);
      vglCl3dSum(buf, dst, dst);
    }
    else if (i % 6 == 4){
      vglCl3dErode(buf, buf2, strel1, 1, 1, 3);
      vglCl3dSum(buf2, dst, dst);
    }
    else{
      vglCl3dErode(buf2, buf, strel2, 1, 1, 3);
      vglCl3dSum(buf, dst, dst);
    }
  }
}

void vglClDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times){
  
  if(src->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with more than 2 dimensions not supported. Use vglCl3dDistTransform instead.\n", __FILE__, __FUNCTION__);
    return;
  }
  
  vglClThreshold(src, buf, 0.0, 1.0f/256.0);
  vglClCopy(buf, dst);
  float square_strel[9] = {1,1,1,1,1,1,1,1,1};
  float cross_strel[9] = {0,1,0,1,1,1,0,1,0};
  for(int i = 0; i < times; i++){
    if (i % 2 == 0){
      vglClErode(buf, buf2,square_strel,3,3);
      vglClSum(buf2, dst, dst);
    }
    else{
      vglClErode(buf2, buf,cross_strel,3,3);
      vglClSum(buf, dst, dst);
    }
  }
}

void vglClTopHat(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, float* strel, int strel_size_x, int strel_size_y, int times)
{

  vglClErode(src, dst, buf, strel, strel_size_x, strel_size_y, times);
  vglClDilate(dst, buf, buf2, strel, strel_size_x, strel_size_y, times);

  vglClSub(src, buf, dst);
}

void vglCl3dTopHat(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int times)
{

  vglCl3dErode(src, dst, buf, strel, strel_size_x, strel_size_y, strel_size_z, times);
  vglCl3dDilate(dst, buf, buf2, strel, strel_size_x, strel_size_y, strel_size_z, times);

  vglCl3dSub(src, buf, dst);
}

/* //TODO: reomove these 2 functions
bool vglCl3dEqual(VglImage* input1, VglImage* input2)
{

  return vglClBinEqual(input1, input2, "vglCl3dEqual");

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
    err = clBuildProgram(program, 1, cl.deviceId, "-I CL_BIN/", NULL, NULL );
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

  size_t worksize[] = { input1->getWidthIn(), input1->getHeightIn(), input1->getLength() };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  
  err = clEnqueueReadBuffer(cl.commandQueue,mobj_equal,CL_TRUE,0,sizeof(char),&e,0,NULL,NULL);
  vglClCheckError( err, (char*) "clEnqueueReadBuffer" );
  
  return e != 1;
}

bool vglClEqual(VglImage* input1, VglImage* input2)
{

  return vglClBinEqual(input1, input2, "vglClEqual");

  if(input1->ndim > 2)
  {
    fprintf(stderr, "%s: %s: Error: image with more than 2 dimensions not supported. Use vglCl3dEqual instead.\n", __FILE__, __FUNCTION__);
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
    err = clBuildProgram(program, 1, cl.deviceId, "-I CL_BIN/", NULL, NULL );
    vglClBuildDebug(err, program);
  }

  static cl_kernel kernel = NULL;
  if (kernel == NULL)
  {
    kernel = clCreateKernel( program, "vglClEqual", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &input1->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &input2->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (void*) &mobj_equal );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  size_t worksize[] = { input1->getWidthIn(), input1->getHeightIn(), 0 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
  
  err = clEnqueueReadBuffer(cl.commandQueue,mobj_equal,CL_TRUE,0,sizeof(char),&e,0,NULL,NULL);
  vglClCheckError( err, (char*) "clEnqueueReadBuffer" );

  return e != 1;
}
*/

bool vglClEqual(VglImage* input1, VglImage* input2)
{
  return vglClEqual(input1, input2, "vglClEqual");
}

bool vglCl3dEqual(VglImage* input1, VglImage* input2)
{
  return vglClEqual(input1, input2, "vglCl3dEqual");
}

bool vglClBinEqual(VglImage* input1, VglImage* input2)
{
  return vglClEqual(input1, input2, "vglClBinEqual");
}

bool vglCl3dBinEqual(VglImage* input1, VglImage* input2)
{
  return vglClEqual(input1, input2, "vglCl3dBinEqual");
}

bool vglClNdBinEqual(VglImage* input1, VglImage* input2)
{
  return vglClEqual(input1, input2, "vglClNdBinEqual");
}

bool vglClEqual(VglImage* input1, VglImage* input2, const char* kernel_name)
{

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
    err = clBuildProgram(program, 1, cl.deviceId, "-I CL_BIN/" , NULL, NULL );
    vglClBuildDebug(err, program);
  }

  static cl_kernel kernel = NULL;
  if (kernel == NULL)
  {
    kernel = clCreateKernel( program, kernel_name, &err );
    vglClCheckError(err, (char*) "clCreateKernel" );
  }

  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &input1->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &input2->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (void*) &mobj_equal );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  int _ndim = 2;
  if (input1->ndim > 2){
    _ndim = 3;
  }

  size_t _worksize_0 = input1->getWidthIn();
  if (input1->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = input1->getWidthStepWords();
  }
  if (input2->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = input2->getWidthStepWords();
  }

  size_t worksize[] = { _worksize_0, input1->getHeightIn(),  input1->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clEnqueueReadBuffer(cl.commandQueue,mobj_equal,CL_TRUE,0,sizeof(char),&e,0,NULL,NULL);
  vglClCheckError( err, (char*) "clEnqueueReadBuffer" );

  return e != 1;
}

/** Conditional dilation by parameterized structuring element.
    All input images must not be the same.
    Image in src is dilated by strel. The minimum between the result and mask is returned.

    The structuring element is a strel_size_x by strel_size_y by strel_size_z parallelepiped.
*/
void vglCl3dConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int strel_size_z)
{

  if(src->ndim != 3)
  {
    fprintf(stderr, "%s: %s: Error: only images with 3 dimensions supported. Use vglClConditionalDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglCl3dDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
  vglCl3dMin(mask, dst, dst);
  
}

/** Conditional dilation by parameterized structuring element. 
    All input images must not be the same.
    Image in src is dilated by strel. The minimum between the result and mask is returned.

    The structuring element is a strel_size_x by strel_size_y rectangle.
*/
void vglClConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, float* strel, int strel_size_x, int strel_size_y)
{

  if(src->ndim != 2)
  {
    fprintf(stderr, "%s: %s: Error: only images with 2 dimensions supported. Use vglCl3dConditionalDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglClDilate(src, dst, strel, strel_size_x, strel_size_y);
  vglClMin(mask, dst, dst);
  
}

/** Conditional erosion by parameterized structuring element.
    All input images must not be the same.
    Image in src is erosed by strel. The maximum between the result and mask is returned.

    The structuring element is a strel_size_x by strel_size_y by strel_size_z parallelepiped.
*/
void vglCl3dConditionalErode(VglImage* src, VglImage* mask, VglImage* dst,  float* strel, int strel_size_x, int strel_size_y, int strel_size_z)
{

  if(src->ndim != 3)
  {
    fprintf(stderr, "%s: %s: Error: only images with 3 dimensions supported. Use vglClConditionalDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglCl3dErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
  vglCl3dMax(mask, dst, dst);
  
}

/** Conditional erosion by parameterized structuring element. 
    All input images must not be the same.
    Image in src is eroded by strel. The maximum between the result and mask is returned.

    The structuring element is a strel_size_x by strel_size_y rectangle.
*/
void vglClConditionalErode(VglImage* src, VglImage* mask, VglImage* dst,  float* strel, int strel_size_x, int strel_size_y)
{

  if(src->ndim != 2)
  {
    fprintf(stderr, "%s: %s: Error: only images with 2 dimensions supported. Use vglCl3dConditionalDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  vglClErode(src, dst, strel, strel_size_x, strel_size_y);
  vglClMax(mask, dst, dst);
}

/** Conditional dilation by parameterized structuring element.
    A buffer is required.
    All input images must not be the same.
    Image in src is dilated by strel. The minimum between the result and mask is returned.

    The structuring element is a strel_size_x by strel_size_y by strel_size_z parallelepiped.
    The parameter "times" indicates how many times the dilation will be applied.
*/
void vglCl3dConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int times)
{
    if (times > 0)
    {
      vglCl3dConditionalDilate(src, mask, buff, strel, strel_size_x, strel_size_y, strel_size_z);

      for(int i = 1; i < times; i++)
      {
        if (i % 2 == 0)
          vglCl3dConditionalDilate(dst, mask, buff, strel, strel_size_x, strel_size_y, strel_size_z);
        else
          vglCl3dConditionalDilate(buff, mask, dst, strel, strel_size_x, strel_size_y, strel_size_z);
      }

      if (times % 2 == 0)
        vglCl3dCopy(buff, dst);
    }
}

/** Conditional dilation by parameterized structuring element. 
    A buffer is required.
    All input images must not be the same.
    Image in src is dilated by strel. The minimum between the result and mask is returned.

    The structuring element is a strel_size_x by strel_size_y rectangle.
    The parameter "times" indicates how many times the dilation will be applied.
*/
void vglClConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int times)
{
    if (times > 0)
    {
      vglClConditionalDilate(src, mask, buff, strel, strel_size_x, strel_size_y);

      for(int i = 1; i < times; i++)
      {
        if (i % 2 == 0)
           vglClConditionalDilate(dst, mask, buff, strel, strel_size_x, strel_size_y);
        else
          vglClConditionalDilate(buff, mask, dst, strel, strel_size_x, strel_size_y);
      }

      if (times % 2 == 0)
        vglCl3dCopy(buff, dst);
    }
}

/** Conditional erosion by parameterized structuring element.
    A buffer is required.
    All input images must not be the same.
    Image in src is erosed by strel. The maximum between the result and mask is returned.

    The structuring element is a strel_size_x by strel_size_y by strel_size_z parallelepiped.
    The parameter "times" indicates how many times the erosion will be applied.
*/
void vglCl3dConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int times)
{
    if (times > 0)
    {
      vglCl3dConditionalErode(src, mask, buff, strel, strel_size_x, strel_size_y, strel_size_z);

      for(int i = 1; i < times; i++)
      {
        if (i % 2 == 0)
           vglCl3dConditionalErode(dst, mask, buff, strel, strel_size_x, strel_size_y, strel_size_z);
        else
          vglCl3dConditionalErode(buff, mask, dst, strel, strel_size_x, strel_size_y, strel_size_z);
      }

      if (times % 2 == 0)
        vglCl3dCopy(buff, dst);
    }
}

/** Conditional erosion by parameterized structuring element. 
    A buffer is required.
    All input images must not be the same.
    Image in src is eroded by strel. The maximum between the result and mask is returned.

    The structuring element is a strel_size_x by strel_size_y rectangle.
    The parameter "times" indicates how many times the erosion will be applied.
*/
void vglClConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y, int times)
{
    if (times > 0)
    {
      vglClConditionalErode(src, mask, buff, strel, strel_size_x, strel_size_y);

      for(int i = 1; i < times; i++)
      {
        if (i % 2 == 0)
           vglClConditionalErode(dst, mask, buff, strel, strel_size_x, strel_size_y);
        else
          vglClConditionalErode(buff, mask, dst, strel, strel_size_x, strel_size_y);
      }

      if (times % 2 == 0)
        vglCl3dCopy(buff, dst);
    }
}


/** Reconstruction by dilation by parameterized structuring element.
    A buffer is required.
    All input images must not be the same.
    Image in marker is dilated by strel, conditioned by src. This dilation is done until stability.

    The structuring element is a strel_size_x by strel_size_y by strel_size_z parallelepiped.
*/
void vglCl3dReconstructionByDilation(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y,int strel_size_z)
{
  vglCl3dConditionalDilate(marker, src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
  vglCl3dCopy(marker, buff);

  int c = 0;
  while(!vglCl3dEqual(dst, buff))
  {
    if (c % 2 == 0)
    {
      vglCl3dConditionalDilate(dst, src, buff, strel, strel_size_x, strel_size_y, strel_size_z);
    }
    else
    {
      vglCl3dConditionalDilate(buff, src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    }
    c++;
  }
}

/** Reconstruction by dilation by parameterized structuring element.
    A buffer is required.
    All input images must not be the same.
    Image in marker is dilated by strel, conditioned by src. This dilation is done until stability.

    The structuring element is a strel_size_x by strel_size_y rectangle.
*/
void vglClReconstructionByDilation(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y)
{
  vglClConditionalDilate(marker, src, dst, strel, strel_size_x, strel_size_y);
  vglClCopy(marker, buff);

  int c = 0;
  while(!vglClEqual(dst, buff))
  {
    if (c % 2 == 0)
    {
      vglClConditionalDilate(dst, src, buff, strel, strel_size_x, strel_size_y);
    }
    else
    {
      vglClConditionalDilate(buff, src, dst, strel, strel_size_x, strel_size_y);
    }
    c++;
  }
}

/** Reconstruction by erosion by parameterized structuring element.
    A buffer is required.
    All input images must not be the same.
    Image in marker is erosed by strel, conditioned by src. This erosion is done until stability.

    The structuring element is a strel_size_x by strel_size_y by strel_size_z parallelepiped.
*/
void vglCl3dReconstructionByErosion(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y,int strel_size_z)
{
  vglCl3dConditionalErode(marker, src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
  vglCl3dCopy(marker, buff);

  int c = 0;
  while(!vglCl3dEqual(dst, buff))
  {
    if (c % 2 == 0)
    {
      vglCl3dConditionalErode(dst, src, buff, strel, strel_size_x, strel_size_y, strel_size_z);
    }
    else
    {
      vglCl3dConditionalErode(buff, src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    }
    c++;
  }
}

/** Reconstruction by erosion by parameterized structuring element.
    A buffer is required.
    All input images must not be the same.
    Image in marker is erosed by strel, conditioned by src. This erosion is done until stability.

    The structuring element is a strel_size_x by strel_size_y rectangle.
*/
void vglClReconstructionByErosion(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, float* strel, int strel_size_x, int strel_size_y)
{
  vglClConditionalErode(marker, src, dst, strel, strel_size_x, strel_size_y);
  vglClCopy(marker, buff);

  int c = 0;
  while(!vglClEqual(dst, buff))
  {
    if (c % 2 == 0)
    {
      vglClConditionalErode(dst, src, buff, strel, strel_size_x, strel_size_y);
    }
    else
    {
      vglClConditionalErode(buff, src, dst, strel, strel_size_x, strel_size_y);
    }
    c++;
  }
}

/** Reconstruction by opening by parameterized structuring element.

    Two buffers are required.
    All input images must not be the same.
    Image in src is eroded by strel. The result is stored in buff. 
    Inage in buff is then used as marker in a reconstruction by dilation.

    The structuring element is a strel_size_x by strel_size_y by strel_size_z parallelepiped.
*/
void vglCl3dReconstructionByOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, float* strel, int strel_size_x, int strel_size_y, int strel_size_z)
{
  vglCl3dErode(src, buff, strel, strel_size_x, strel_size_y, strel_size_z);
  vglCl3dReconstructionByDilation(src, buff, dst, buff2, strel, strel_size_x, strel_size_y, strel_size_z);
}

/** Reconstruction by opening by parameterized structuring element.

    Two buffers are required.
    All input images must not be the same.
    Image in src is eroded by strel. The result is stored in buff. 
    Inage in buff is then used as marker in a reconstruction by dilation.

    The structuring element is a strel_size_x by strel_size_y rectangle.
*/
void vglClReconstructionByOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, float* strel, int strel_size_x, int strel_size_y)
{
  vglClErode(src, buff, strel, strel_size_x, strel_size_y);
  vglClReconstructionByDilation(src, buff, dst, buff2, strel, strel_size_x, strel_size_y);
}

/** Reconstruction by closing by parameterized structuring element.
    Two buffers are required.
    All input images must not be the same.
    Image in src is dilated by strel. The result is stored in buff. 
    Inage in buff is then used as marker in a reconstruction by erosion.

    The structuring element is a strel_size_x by strel_size_y by strel_size_z parallelepiped.
*/
void vglCl3dReconstructionByClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, float* strel, int strel_size_x, int strel_size_y, int strel_size_z)
{
  vglCl3dDilate(src, buff, strel, strel_size_x, strel_size_y, strel_size_z);
  vglCl3dReconstructionByErosion(src, buff, dst, buff2, strel, strel_size_x, strel_size_y, strel_size_z);
}

/** Reconstruction by closing by parameterized structuring element.
    Two buffers are required.
    All input images must not be the same.
    Image in src is dilated by strel. The result is stored in buff. 
    Inage in buff is then used as marker in a reconstruction by erosion.

    The structuring element is a strel_size_x by strel_size_y rectangle.
*/
void vglClReconstructionByClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, float* strel, int strel_size_x, int strel_size_y)
{
  vglClDilate(src, buff, strel, strel_size_x, strel_size_y);
  vglClReconstructionByErosion(src, buff, dst, buff2, strel, strel_size_x, strel_size_y);
}

void vglClFuzzyErode(VglImage* src, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int type, float gamma)
{

  if (src->ndim != 2)
  {
    fprintf(stderr, "%s: %s: Error: only images with 2 dimensions supported. Use vglCl3dFuzzyErode instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  switch (type)
  {
    case VGL_CL_FUZZY_ALGEBRAIC:
      vglClFuzzyAlgErode(src, dst, strel, strel_size_x, strel_size_y);
      break;
    case VGL_CL_FUZZY_ARITHMETIC:
      vglClFuzzyArithErode(src, dst, strel, strel_size_x, strel_size_y);
      break;
    case VGL_CL_FUZZY_BOUNDED:
      vglClFuzzyBoundErode(src, dst, strel, strel_size_x, strel_size_y);
      break;
    case VGL_CL_FUZZY_DAP:
      vglClFuzzyDaPErode(src, dst, strel, strel_size_x, strel_size_y, gamma);
      break;
    case VGL_CL_FUZZY_DRASTIC:
      vglClFuzzyDrasticErode(src, dst, strel, strel_size_x, strel_size_y);
      break;
    case VGL_CL_FUZZY_GEOMETRIC:
      vglClFuzzyGeoErode(src, dst, strel, strel_size_x, strel_size_y);
      break;
    case VGL_CL_FUZZY_HAMACHER:
      vglClFuzzyHamacherErode(src, dst, strel, strel_size_x, strel_size_y, gamma);
      break;
    case VGL_CL_FUZZY_STANDARD:
      vglClFuzzyStdErode(src, dst, strel, strel_size_x, strel_size_y);
      break;
    default:
      printf("%s: %s : Warning: Not a valid type of operation\n", __FILE__, __FUNCTION__);
  }
}

void vglClFuzzyDilate(VglImage* src, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int type, float gamma)
{

  if (src->ndim != 2)
  {
    fprintf(stderr, "%s: %s: Error: only images with 2 dimensions supported. Use vglCl3dFuzzyDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  switch (type)
  {
  case VGL_CL_FUZZY_ALGEBRAIC:
    vglClFuzzyAlgDilate(src, dst, strel, strel_size_x, strel_size_y);
    break;
  case VGL_CL_FUZZY_ARITHMETIC:
    vglClFuzzyArithDilate(src, dst, strel, strel_size_x, strel_size_y);
    break;
  case VGL_CL_FUZZY_BOUNDED:
    vglClFuzzyBoundDilate(src, dst, strel, strel_size_x, strel_size_y);
    break;
  case VGL_CL_FUZZY_DAP:
    vglClFuzzyDaPDilate(src, dst, strel, strel_size_x, strel_size_y, gamma);
    break;
  case VGL_CL_FUZZY_DRASTIC:
    vglClFuzzyDrasticDilate(src, dst, strel, strel_size_x, strel_size_y);
    break;
  case VGL_CL_FUZZY_GEOMETRIC:
    vglClFuzzyGeoDilate(src, dst, strel, strel_size_x, strel_size_y);
    break;
  case VGL_CL_FUZZY_HAMACHER:
    vglClFuzzyHamacherDilate(src, dst, strel, strel_size_x, strel_size_y, gamma);
    break;
  case VGL_CL_FUZZY_STANDARD:
    vglClFuzzyStdDilate(src, dst, strel, strel_size_x, strel_size_y);
    break;
  default:
    printf("%s: %s : Warning: Not a valid type of operation\n", __FILE__, __FUNCTION__);
  }
}

void vglCl3dFuzzyErode(VglImage* src, VglImage* dst, float* strel, int strel_size_x, int strel_size_y, int strel_size_z, int type, float gamma)
{

  if (src->ndim != 3)
  {
    fprintf(stderr, "%s: %s: Error: only images with 3 dimensions supported. Use vglClFuzzyErode instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  switch (type)
  {
  case VGL_CL_FUZZY_ALGEBRAIC:
    vglCl3dFuzzyAlgErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_ARITHMETIC:
    vglCl3dFuzzyArithErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_BOUNDED:
    vglCl3dFuzzyBoundErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_DAP:
    vglCl3dFuzzyDaPErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z, gamma);
    break;
  case VGL_CL_FUZZY_DRASTIC:
    vglCl3dFuzzyDrasticErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_GEOMETRIC:
    vglCl3dFuzzyGeoErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_HAMACHER:
    vglCl3dFuzzyHamacherErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z, gamma);
    break;
  case VGL_CL_FUZZY_STANDARD:
    vglCl3dFuzzyStdErode(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  default:
    printf("%s: %s : Warning: Not a valid type of operation\n", __FILE__, __FUNCTION__);
  }
}

void vglCl3dFuzzyDilate(VglImage* src, VglImage* dst, float* strel, int strel_size_x, int strel_size_y,int strel_size_z, int type, float gamma)
{

  if (src->ndim != 3)
  {
    fprintf(stderr, "%s: %s: Error: only images with 3 dimensions supported. Use vglClFuzzyDilate instead.\n", __FILE__, __FUNCTION__);
    return;
  }

  switch (type)
  {
  case VGL_CL_FUZZY_ALGEBRAIC:
    vglCl3dFuzzyAlgDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_ARITHMETIC:
    vglCl3dFuzzyArithDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_BOUNDED:
    vglCl3dFuzzyBoundDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_DAP:
    vglCl3dFuzzyDaPDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z, gamma);
    break;
  case VGL_CL_FUZZY_DRASTIC:
    vglCl3dFuzzyDrasticDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_GEOMETRIC:
    vglCl3dFuzzyGeoDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  case VGL_CL_FUZZY_HAMACHER:
    vglCl3dFuzzyHamacherDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z, gamma);
    break;
  case VGL_CL_FUZZY_STANDARD:
    vglCl3dFuzzyStdDilate(src, dst, strel, strel_size_x, strel_size_y, strel_size_z);
    break;
  default:
    printf("%s: %s : Warning: Not a valid type of operation. type = %d\n", __FILE__, __FUNCTION__, type);
  }
}

/** Binary 2D dilation of src by strel. Result is stored in dst.

    This version of dilation masks vglClBinDilatePack, that receives an array instead of VglStrEl.
  */
void vglClBinDilate(VglImage* src, VglImage* dst, VglStrEl* strel)
{
  vglClBinDilatePack(src, dst, strel->data, strel->vglShape->shape[VGL_SHAPE_WIDTH], strel->vglShape->shape[VGL_SHAPE_HEIGHT]);
}

/** Binary 3D dilation of src by strel. Result is stored in dst.

    This version of dilation masks vglCl3dBinDilatePack, that receives an array instead of VglStrEl.
  */
void vglCl3dBinDilate(VglImage* src, VglImage* dst, VglStrEl* strel)
{
  vglCl3dBinDilatePack(src, dst, strel->data, strel->vglShape->shape[VGL_SHAPE_WIDTH], strel->vglShape->shape[VGL_SHAPE_HEIGHT], strel->vglShape->shape[VGL_SHAPE_LENGTH]);
}

/** Binary 2D erosion of src by strel. Result is stored in dst.

    This version of erosion masks vglClBinErodePack, that receives an array instead of VglStrEl.
  */
void vglClBinErode(VglImage* src, VglImage* dst, VglStrEl* strel)
{
  vglClBinErodePack(src, dst, strel->data, strel->vglShape->shape[VGL_SHAPE_WIDTH], strel->vglShape->shape[VGL_SHAPE_HEIGHT]);
}

/** Binary 3D erosion of src by strel. Result is stored in dst.

    This version of erosion masks vglCl3dBinErodePack, that receives an array instead of VglStrEl.
  */
void vglCl3dBinErode(VglImage* src, VglImage* dst, VglStrEl* strel)
{
  vglCl3dBinErodePack(src, dst, strel->data, strel->vglShape->shape[VGL_SHAPE_WIDTH], strel->vglShape->shape[VGL_SHAPE_HEIGHT], strel->vglShape->shape[VGL_SHAPE_LENGTH]);
}


/** N dilation by structuring element.

    All input images must not be the same.
    Image in src is dilated by strel as many times as defined.
*/
void vglClBinNDilate(VglImage* src, VglImage* dst, VglImage* buff, VglStrEl* strel, int times)
{
  #define BIN_DILATE(funcDilate, funcCopy)       \
  if (times > 0)                                 \
  {                                              \
    funcDilate(src, buff, strel);                \
    for(int i = 1; i < times; i++)               \
    {                                            \
      if (i % 2 == 0)                            \
        funcDilate(dst, buff, strel);            \
      else                                       \
        funcDilate(buff, dst, strel);            \
    }                                            \
    if (times % 2 == 1)                          \
      funcCopy(buff, dst);                       \
  }                                              \

  if (!src->clForceAsBuf)
  {
    if (src->ndim == 2)
    {
      BIN_DILATE(vglClBinDilate, vglClBinCopy)
    }
    else if (src->ndim == 3)
    {
      BIN_DILATE(vglCl3dBinDilate, vglCl3dBinCopy)
    }
  }
  else
  {
    BIN_DILATE(vglClNdBinDilate, vglClNdBinCopy)
  }
}

/** N erosion by structuring element.

    All input images must not be the same.
    Image in src is eroded by strel as many times as defined.
*/
void vglClBinNErode(VglImage* src, VglImage* dst, VglImage* buff, VglStrEl* strel, int times)
{
  #define BIN_ERODE(funcErode, funcCopy)         \
  if (times > 0)                                 \
  {                                              \
    funcErode(src, buff, strel);                 \
    for(int i = 1; i < times; i++)               \
    {                                            \
      if (i % 2 == 0)                            \
        funcErode(dst, buff, strel);	         \
      else                                       \
        funcErode(buff, dst, strel);             \
    }                                            \
    if (times % 2 == 1)                          \
      funcCopy(buff, dst);                       \
  }                                              \

  if (!src->clForceAsBuf)
  {
    if (src->ndim == 2)
    {
      BIN_ERODE(vglClBinErode, vglClBinCopy)
    }
    else if (src->ndim == 3)
    {
      BIN_ERODE(vglCl3dBinErode, vglCl3dBinCopy)
    }
  }
  else
  {
    BIN_ERODE(vglClNdBinErode, vglClNdBinCopy)
  }    
}


/** Conditional dilation by parameterized structuring element.

    All input images must not be the same.
    Image in src is dilated by strel. The minimum between the result and mask is returned.
*/
void vglClBinConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, VglStrEl* strel)
{
  #define BIN_COND_DILATE(funcDilate, funcMin)  \
      funcDilate(src, dst, strel);              \
      funcMin(mask, dst, dst);                  \

  if (!src->clForceAsBuf)
  {
    if (src->ndim == 2)
    {
      BIN_COND_DILATE(vglClBinDilate, vglClBinMin)
    }
    else if (src->ndim == 3)
    {
      BIN_COND_DILATE(vglCl3dBinDilate, vglCl3dBinMin)
    }
  }
  else
  {
    BIN_COND_DILATE(vglClNdBinDilate, vglClNdBinMin)
  }
}

/** Conditional erosion by parameterized structuring element.

    All input images must not be the same.
    Image in src is eroded by strel. The maximum between the result and mask is returned.
*/
void vglClBinConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, VglStrEl* strel)
{
  #define BIN_COND_ERODE(funcErode, funcMax)    \
      funcErode(src, dst, strel);               \
      funcMax(mask, dst, dst);                  \

  if (!src->clForceAsBuf)
  {
    if (src->ndim == 2)
    {
      BIN_COND_ERODE(vglClBinErode, vglClBinMax)
    }
    else if (src->ndim == 3)
    {
      BIN_COND_ERODE(vglCl3dBinErode, vglCl3dBinMax)
    }
  }
  else
  {
    BIN_COND_ERODE(vglClNdBinErode, vglClNdBinMax)
  }
}

/** Opening by structuring element.

    Two buffers are required.
    All input images must not be the same.
*/
void vglClBinOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel, int times)
{
  vglClBinNErode(src, buff2, buff, strel, 1);
  vglClBinNDilate(buff2, dst, buff, strel, 1);
}

/** Closing by structuring element.

    Two buffers are required.
    All input images must not be the same.
*/

void vglClBinClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel, int times)
{
  if (times > 0)
  {
    vglClBinNDilate(src, buff2, buff, strel, 1);
    vglClBinNErode(buff2, dst, buff, strel, 1);
  }
}
/** N Opening by structuring element.

    Two buffers are required.
    All input images must not be the same.
    Image in src is opened by strel by doing as many dilations and erosions as defined.
*/
void vglClBinNOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel, int times)
{
  if (times > 0)
  {
    vglClBinNErode(src, buff2, buff, strel, times);
    vglClBinNDilate(buff2, dst, buff, strel, times);
  }
}

/** N Closing by structuring element.

    Two buffers are required.
    All input images must not be the same.
    Image in src is closed by strel by doing as many dilations and erosions as defined.
*/
void vglClBinNClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel, int times)
{
  if (times > 0)
  {
    vglClBinNDilate(src, buff2, buff, strel, times);
    vglClBinNErode(buff2, dst, buff, strel, times);
  }
}

/** Binary copy

    Choose proper function depending on image dimensions and if is buffer or not.
*/
void vglClXdBinCopy(VglImage* src, VglImage* dst)
{
  if (!src->clForceAsBuf)
  {
    if (src->ndim == 2)
    {
      vglClBinCopy(src, dst);
    }
    else if (src->ndim == 3)
    {
      vglCl3dBinCopy(src, dst);
    }
  }
  else
  {
    vglClNdBinCopy(src, dst);
  }
}

/** Binary comparison

    Choose proper function depending on image dimensions and if is buffer or not.
*/
bool vglClXdBinEqual(VglImage* input1, VglImage* input2)
{
  if (!input1->clForceAsBuf)
  {
    if (input1->ndim == 2)
    {
      return vglClBinEqual(input1, input2);
    }
    else if (input1->ndim == 3)
    {
      return vglCl3dBinEqual(input1, input2);
    }
  }
  else
  {
    return vglClNdBinEqual(input1, input2);
  }
}

/** Binary reconstruction by dilation by structuring element.

    A buffer is required.
    All input images must not be the same.
    Image in marker is dilated by strel, conditioned by src. This dilation is done until stability.
*/
void vglClBinReconstructionByDilation(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, VglStrEl* strel)
{
  vglClBinConditionalDilate(marker, src, dst, strel);
  vglClXdBinCopy(marker, buff);

  int c = 0;
  while(!vglClXdBinEqual(dst, buff))
  {
    if (c % 2 == 0)
    {
      vglClBinConditionalDilate(dst, src, buff, strel);
    }
    else
    {
      vglClBinConditionalDilate(buff, src, dst, strel);
    }
    c++;
  }
}

/** Binary reconstruction by erosion by structuring element.

    A buffer is required.
    All input images must not be the same.
    Image in marker is eroded by strel, conditioned by src. This erosion is done until stability.
*/
void vglClBinReconstructionByErosion(VglImage* src, VglImage* marker, VglImage* dst, VglImage* buff, VglStrEl* strel)
{
  vglClBinConditionalErode(marker, src, dst, strel);
  vglClXdBinCopy(marker, buff);

  int c = 0;
  while(!vglClXdBinEqual(dst, buff))
  {
    if (c % 2 == 0)
    {
      vglClBinConditionalErode(dst, src, buff, strel);
    }
    else
    {
      vglClBinConditionalErode(buff, src, dst, strel);
    }
    c++;
  }
}

/** Binary reconstruction by opening by parameterized structuring element.

    Two buffers are required.
    All input images must not be the same.
    Image in src is eroded by strel. The result is stored in buff.
    Inage in buff is then used as marker in a reconstruction by dilation.
*/
void vglClBinReconstructionByOpening(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel)
{
  vglClBinNErode(src, buff, buff2, strel, 1);
  vglClBinReconstructionByDilation(src, buff, dst, buff2, strel);
}

/** Binary reconstruction by closing by parameterized structuring element.

    Two buffers are required.
    All input images must not be the same.
    Image in src is dilated by strel. The result is stored in buff.
    Inage in buff is then used as marker in a reconstruction by erosion.
*/
void vglClBinReconstructionByClosing(VglImage* src, VglImage* dst, VglImage* buff, VglImage* buff2, VglStrEl* strel)
{
  vglClBinNDilate(src, buff, buff2, strel, 1);
  vglClBinReconstructionByErosion(src, buff, dst, buff2, strel);
}


/** Binary N conditional dilation by structuring element.

    All input images must not be the same.
    Image in src is dilated by strel conditioned by mask as many times as defined.
*/
void vglClBinNConditionalDilate(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, VglStrEl* strel, int times)
{
  if (times > 0)
  {
    vglClBinConditionalDilate(src, mask, buff, strel);
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglClBinConditionalDilate(dst, mask, buff, strel);
      else
        vglClBinConditionalDilate(buff, mask, dst, strel);
    }
    if (times % 2 == 1)
      vglClXdBinCopy(buff, dst);
  }
}

/** Binary N conditional erosion by structuring element.

    All input images must not be the same.
    Image in src is eroded by strel conditioned by mask as many times as defined.
*/
void vglClBinNConditionalErode(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buff, VglStrEl* strel, int times)
{
  if (times > 0)
  {
    vglClBinConditionalErode(src, mask, buff, strel);
    for(int i = 1; i < times; i++)
    {
      if (i % 2 == 0)
        vglClBinConditionalErode(dst, mask, buff, strel);
      else
        vglClBinConditionalErode(buff, mask, dst, strel);
    }
    if (times % 2 == 1)
      vglClXdBinCopy(buff, dst);
  }
}
