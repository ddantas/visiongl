
/*********************************************************************
***                                                                 ***
***  Source code generated by cl2cpp.pl                             ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/

#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"


#include <fstream>

extern VglClContext cl;

/** Convolution of src image by mask. Result is stored in dst image.

  */
void vglClBlurSq3(VglImage* img_input, VglImage* img_output){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int err;

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClBlurSq3.cl";
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
    kernel = clCreateKernel( program, "vglClBlurSq3", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  size_t worksize[] = { img_input->width, img_input->height, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Convolution of src image by mask. Result is stored in dst image.

  */
void vglClConvolution(VglImage* img_input, VglImage* img_output, float* convolution_window, int window_size_x, int window_size_y){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_convolution_window = NULL;
  mobj_convolution_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (window_size_x*window_size_y)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_convolution_window, CL_TRUE, 0, (window_size_x*window_size_y)*sizeof(float), convolution_window, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

  cl_mem mobj_window_size_x = NULL;
  mobj_window_size_x = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (1)*sizeof(int), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer window_size_x" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_window_size_x, CL_TRUE, 0, (1)*sizeof(int), &window_size_x, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer window_size_x" );

  cl_mem mobj_window_size_y = NULL;
  mobj_window_size_y = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (1)*sizeof(int), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer window_size_y" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_window_size_y, CL_TRUE, 0, (1)*sizeof(int), &window_size_y, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer window_size_y" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClConvolution.cl";
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
    kernel = clCreateKernel( program, "vglClConvolution", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_convolution_window );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  err = clSetKernelArg( kernel, 3, sizeof( cl_mem ), (int*) &mobj_window_size_x );
  vglClCheckError( err, (char*) "clSetKernelArg 3" );

  err = clSetKernelArg( kernel, 4, sizeof( cl_mem ), (int*) &mobj_window_size_y );
  vglClCheckError( err, (char*) "clSetKernelArg 4" );

  size_t worksize[] = { img_input->width, img_input->height, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_convolution_window );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_convolution_window");

  err = clReleaseMemObject( mobj_window_size_x );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_window_size_x");

  err = clReleaseMemObject( mobj_window_size_y );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_window_size_y");

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Direct copy from src to dst.

  */
void vglClCopy(VglImage* img_input, VglImage* img_output){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int err;

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClCopy.cl";
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
    kernel = clCreateKernel( program, "vglClCopy", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  size_t worksize[] = { img_input->width, img_input->height, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Negative of src image. Result is stored in dst image.

  */
void vglClInvert(VglImage* img_input, VglImage* img_output){

  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int err;

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClInvert.cl";
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
    kernel = clCreateKernel( program, "vglClInvert", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  size_t worksize[] = { img_input->width, img_input->height, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Threshold of src image by float parameter. Result is stored in dst image.

  */
void vglClThreshold(VglImage* src, VglImage* dst, float thresh){

  vglCheckContext(src, VGL_CL_CONTEXT);
  vglCheckContext(dst, VGL_CL_CONTEXT);

  cl_int err;

  cl_mem mobj_thresh = NULL;
  mobj_thresh = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, (1)*sizeof(float), NULL, &err);
  vglClCheckError( err, (char*) "clCreateBuffer thresh" );
  err = clEnqueueWriteBuffer(cl.commandQueue, mobj_thresh, CL_TRUE, 0, (1)*sizeof(float), &thresh, 0, NULL, NULL);
  vglClCheckError( err, (char*) "clEnqueueWriteBuffer thresh" );

  static cl_program program = NULL;
  if (program == NULL)
  {
    char* file_path = (char*) "CL/vglClThreshold.cl";
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
    kernel = clCreateKernel( program, "vglClThreshold", &err ); 
    vglClCheckError(err, (char*) "clCreateKernel" );
  }


  err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*) &src->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 0" );

  err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void*) &dst->oclPtr );
  vglClCheckError( err, (char*) "clSetKernelArg 1" );

  err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float*) &mobj_thresh );
  vglClCheckError( err, (char*) "clSetKernelArg 2" );

  size_t worksize[] = { src->width, src->height, 1 };
  clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
  vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

  err = clReleaseMemObject( mobj_thresh );
  vglClCheckError(err, (char*) "clReleaseMemObject mobj_thresh");

  vglSetContext(dst, VGL_CL_CONTEXT);
}

