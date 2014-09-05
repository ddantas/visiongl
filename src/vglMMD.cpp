#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"

#include <fstream>

extern VglClContext cl;

/** Erosion of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyAlgErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyAlgErosion.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyAlgErosion", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


/** Dilatation of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyAlgDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyAlgDilation.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyAlgDilation", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

void vglClFuzzyArithErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyArithErosion.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyArithErosion", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


void vglClFuzzyArithDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyArithDilation.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyArithDilation", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

void vglClFuzzyBoundErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyBoundErosion.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyBoundErosion", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


/** Dilatation of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyBoundDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyBoundDilation.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyBoundDilation", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


void vglClFuzzyDrasticErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyDrasticErosion.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyDrasticErosion", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


/** Dilatation of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyDrasticDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyDrasticDilation.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyDrasticDilation", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


void vglClFuzzyGeoErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyGeoErosion.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyGeoErosion", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


/** Dilatation of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyGeoDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyGeoDilation.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyGeoDilation", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Dilatation of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyDaPDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y, float gama){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyDaPDilation.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyDaPDilation", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  err = clSetKernelArg( kernel, 5, sizeof( float ), &gama );
  vglClCheckError( err, (char*) "clSetKernelArg 5" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Dilatation of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyDaPErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y, float gama){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyDaPErosion.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyDaPErosion", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  err = clSetKernelArg( kernel, 5, sizeof( float ), &gama );
  vglClCheckError( err, (char*) "clSetKernelArg 5" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}


/** Dilatation of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyHamacherDilation(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y, float gama){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyHamacherDilation.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyHamacherDilation", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  err = clSetKernelArg( kernel, 5, sizeof( float ), &gama );
  vglClCheckError( err, (char*) "clSetKernelArg 5" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Dilatation of src image by mask. Result is stored in dst image.

  */
void vglClFuzzyHamacherErosion(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y, float gama){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglGlToCl(img_output);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClFuzzyHamacherErosion.cl";
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
    kernel = clCreateKernel( program, "vglClFuzzyHamacherErosion", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( int ), &window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( int ), &window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  err = clSetKernelArg( kernel, 5, sizeof( float ), &gama );
  vglClCheckError( err, (char*) "clSetKernelArg 5" );

  if (img_input->ndim <= 2){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], 1 };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  }
  else if (img_input->ndim == 3){
    size_t worksize[] = { img_input->shape[VGL_WIDTH], img_input->shape[VGL_HEIGHT], img_input->shape[VGL_LENGTH] };
    clEnqueueNDRangeKernel( cl.commandQueue, kernel, 3, NULL, worksize, 0, 0, 0, 0 );
  }
  else{
    printf("More than 3 dimensions not yet supported\n");
  }

  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}