
#ifdef __OPENCL__

#include "vglClImage.h"
#include "vglContext.h"


#include <fstream>

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
//#include <opencv2/core/core_c.h>
//#include <opencv2/highgui/highgui_c.h>

static VglClContext cl;

void vglClPrintContext(void)
{
  printf("cl_platform_id* platformId    = %p\n", cl.platformId);
  printf("cl_device_id* deviceId        = %p\n", cl.deviceId);
  printf("cl_context context            = %p\n", cl.context);
  printf("cl_command_queue commandQueue = %p\n", cl.commandQueue);
}


void vglClCheckError(cl_int error, char* name)
{
    if (error != CL_SUCCESS)
    {
        printf("Erro %d while doing the following operation %s\n",error,name);
        system("pause");
	exit(1);
    }
}

void vglClInit()
{
	cl_int err;
	cl_uint num_platforms, num_devices;
	err = clGetPlatformIDs(0, NULL, &num_platforms);
	vglClCheckError(err, (char*) "clGetPlatformIDs get number of platforms");
	cl.platformId = (cl_platform_id*)malloc(sizeof(cl_platform_id)*num_platforms);
	err = clGetPlatformIDs(num_platforms,cl.platformId,NULL);
	vglClCheckError(err, (char*) "clGetPlatformIDs get platforms id");	

	if (num_platforms == 0)
		printf("found no platform for opencl\n\n");
	else if (num_platforms > 1)
	  printf("found %p platforms for opencl\n\n", (unsigned int*) &num_platforms);
	else
		printf("found 1 platform for opencl\n\n");

	err = clGetDeviceIDs(*cl.platformId,CL_DEVICE_TYPE_GPU,0,NULL,&num_devices);
	vglClCheckError(err, (char*) "clGetDeviceIDs get number of devices");
	cl.deviceId = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
	err = clGetDeviceIDs(*cl.platformId,CL_DEVICE_TYPE_GPU,num_devices,cl.deviceId,NULL);
	vglClCheckError(err, (char*) "clGetDeviceIDs get devices id");
	//precisa adicionar a propriedade CL_KHR_gl_sharing no contexto e pra isso precisará do id do contexto do GL que deverá ser o parametro window
	//cl_context_properties props[] =	{CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext()};
	cl.context = clCreateContext(NULL,1,cl.deviceId,NULL, NULL, &err );
	vglClCheckError(err, (char*) "clCreateContext GPU");

	cl.commandQueue = clCreateCommandQueue( cl.context, *cl.deviceId, 0, &err );
	vglClCheckError( err, (char*) "clCreateCommandQueue" );

}

void vglClFlush()
{
	cl_int err;
	err = clFlush( cl.commandQueue );
	vglClCheckError(err, (char*) "clFlush command_queue");
	err = clFinish( cl.commandQueue );
	vglClCheckError(err, (char*) "clFinish command_queue");
	err = clReleaseCommandQueue( cl.commandQueue );
	vglClCheckError(err, (char*) "clReleaseCommandQueue command_queue");
	err = clReleaseContext( cl.context );
	vglClCheckError(err, (char*) "clReleaseContext context");

}

void vglClBuildDebug(cl_int err, cl_program program)
{
	if (err != CL_SUCCESS)
	{
		printf("Error building program\n");

		char buffer[4096];
		size_t length;
		clGetProgramBuildInfo(
			program, // valid program object
			*cl.deviceId, // valid device_id that executable was built
			CL_PROGRAM_BUILD_LOG, // indicate to retrieve build log
			sizeof(buffer), // size of the buffer to write log to
			buffer, // the actual buffer to write log to
			&length // the actual size in bytes of data copied to buffer
		);
		printf("%s\n",buffer);
		system("pause");
		exit(1);
	}
}

void vglClUpload(VglImage* img)
{
    cl_int err;

    if (!vglIsInContext(img, VGL_RAM_CONTEXT)  && 
        !vglIsInContext(img, VGL_BLANK_CONTEXT)    ){
      fprintf(stderr, "vglClUpload: Error: image context = %d not in VGL_RAM_CONTEXT or VGL_BLANK_CONTEXT\n", img->inContext);
      return;
    }

    if (img->oclPtr == NULL)
    {
        /*if (img->fbo != -1)
        {
          img->oclPtr = clCreateFromGLTexture2D(cl.context,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,img->fbo,&err);
          vglClCheckError( err, (char*) "clCreateFromGlTexture2D interop" );
          clEnqueueAcquireGLObjects(cl.commandQueue, 1, &img->oclPtr, 0,0,0);
        }
        else
        {*/

        printf("vglClUpload initializing oclPtr\n");
        cl_image_format format;
        if (img->nChannels == 1)
        {
            format.image_channel_order = CL_R;
            format.image_channel_data_type = CL_UNORM_INT8;
        }
        else
        {
            if (img->iplRGBA == NULL)
            {
                printf("creating RGBA\n");
                img->iplRGBA = cvCreateImage(cvGetSize(img->ipl), IPL_DEPTH_8U, 4);
            }
            format.image_channel_order = CL_RGBA;
            format.image_channel_data_type = CL_UNORM_INT8;
        }
        img->oclPtr = clCreateImage2D(cl.context, CL_MEM_READ_WRITE, &format, img->width, img->height, 0, NULL, &err);
        vglClCheckError( err, (char*) "clCreateImage2D" );
    }
 
    if (vglIsInContext(img, VGL_RAM_CONTEXT))
    {
        cvCvtColor(img->ipl, img->iplRGBA, CV_BGR2RGBA);

        size_t Origin[3] = { 0, 0, 0};
        size_t Size3d[3] = { img->width, img->height, 1 };

        err = clEnqueueWriteImage( cl.commandQueue, img->oclPtr, CL_TRUE, Origin, Size3d,0,0, img->iplRGBA->imageData, 0, NULL, NULL );
        vglClCheckError( err, (char*) "clEnqueueWriteImage" );
    }

    vglAddContext(img, VGL_CL_CONTEXT);
}

void vglClDownload(VglImage* img)
{
    if (!vglIsInContext(img, VGL_CL_CONTEXT))
    {
      fprintf(stderr, "vglClDownload: Error: image context = %d not in VGL_CL_CONTEXT\n", img->inContext);
      return;
    }

    size_t Origin[3] = { 0, 0, 0};
    size_t Size3d[3] = { img->width, img->height, 1 };

    cl_int err_cl = clEnqueueReadImage( cl.commandQueue, img->oclPtr, CL_TRUE, Origin, Size3d, 0, 0, img->iplRGBA->imageData, 0, NULL, NULL );
    vglClCheckError( err_cl, (char*) "clEnqueueReadImage" );
    cvCvtColor(img->iplRGBA, img->ipl, CV_RGBA2BGR);

    vglAddContext(img, VGL_RAM_CONTEXT);
}

void vglClCopy(VglImage* src, VglImage* dst)
{
	cl_int err;

        vglCheckContext(src, VGL_CL_CONTEXT);
        vglCheckContext(dst, VGL_CL_CONTEXT);

	static cl_program program = NULL;
	if (program == NULL){
	  char* file_path = (char*) "CL/vglClCopy.cl";
		std::ifstream file(file_path);
		if(file.fail())
		{
			std::string str("File not found: ");
			str.append(file_path);
			vglClCheckError(-1,(char*)str.c_str());
		}
		std::string prog( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
		const char *source_str = prog.c_str();
#ifdef __DEBUG__
		printf("Kernel to be compiled:\n%s\n", source_str);
#endif

		program = clCreateProgramWithSource( cl.context, 1, (const char **) &source_str, 0, &err );
		vglClCheckError( err, (char*) "clCreateProgramWithSource" );
		
		err = clBuildProgram( program, 1, cl.deviceId, NULL, NULL, NULL );
		vglClBuildDebug(err, program);
	}

	static cl_kernel kernel = NULL;
	if (kernel == NULL)
	{
		kernel = clCreateKernel( program, "copy", &err ); 
		vglClCheckError( err, (char*) "clCreateKernel" );
	}

	err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void *) &src->oclPtr );
	vglClCheckError( err, (char*) "clSetKernelArg A" );
	err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void *) &dst->oclPtr );
	vglClCheckError( err, (char*) "clSetKernelArg B" );

	size_t worksize[] = { src->width, src->height, 1 };
	clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
	vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
	
        vglSetContext(dst, VGL_CL_CONTEXT);
}
void vglClInvert(VglImage* src, VglImage* dst)
{
	cl_int err;
	
        vglCheckContext(src, VGL_CL_CONTEXT);
        vglCheckContext(dst, VGL_CL_CONTEXT);

	static cl_program program = NULL;
	if (program == NULL){
	  char* file_path = (char*) "CL/vglClInvert.cl";
		std::ifstream file(file_path);
		if(file.fail())
		{
			std::string str("File not found: ");
			str.append(file_path);
			vglClCheckError(-1,(char*)str.c_str());
		}
		std::string prog( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
		const char *source_str = prog.c_str();
#ifdef __DEBUG__
		printf("Kernel to be compiled:\n%s\n", source_str);
#endif

		program = clCreateProgramWithSource( cl.context, 1, (const char **) &source_str, 0, &err );
		vglClCheckError( err, (char*) "clCreateProgramWithSource" );
		
		err = clBuildProgram( program, 1, cl.deviceId, NULL, NULL, NULL );
		vglClBuildDebug(err,program);
	}

	static cl_kernel kernel = NULL;
	if (kernel == NULL)
	{
		kernel = clCreateKernel( program, "invert", &err ); 
		vglClCheckError( err, (char*) "clCreateKernel" );
	}

	err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void *) &src->oclPtr );
	vglClCheckError( err, (char*) "clSetKernelArg A" );
	err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void *) &dst->oclPtr );
	vglClCheckError( err, (char*) "clSetKernelArg B" );

	size_t worksize[] = { src->width, src->height, 1 };
	clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
	vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );
	
        vglSetContext(dst, VGL_CL_CONTEXT);
}
void vglClThreshold(VglImage* src, VglImage* dst, float thresh)
{
	cl_int err;
	
        vglCheckContext(src, VGL_CL_CONTEXT);
        vglCheckContext(dst, VGL_CL_CONTEXT);

	cl_mem mobj_C = NULL;
	mobj_C = clCreateBuffer(cl.context,CL_MEM_READ_ONLY,sizeof(thresh),NULL,&err);
	vglClCheckError( err, (char*) "clCreateBuffer thresh" );
	err = clEnqueueWriteBuffer(cl.commandQueue, mobj_C, CL_TRUE, 0, sizeof(thresh), &thresh, 0, NULL, NULL);
	vglClCheckError( err, (char*) "clEnqueueWriteBuffer thresh" );

	static cl_program program = NULL;
	if (program == NULL)
	{
                char* file_path = (char*) "CL/vglClThreshold.cl";
		std::ifstream file(file_path);
		if(file.fail())
		{
			std::string str("File not found: ");
			str.append(file_path);
			vglClCheckError(-1,(char*)str.c_str());
		}
		std::string prog( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
		const char *source_str = prog.c_str();
#ifdef __DEBUG__
		printf("Kernel to be compiled:\n%s\n", source_str);
#endif
		program = clCreateProgramWithSource( cl.context, 1, (const char **) &source_str, 0, &err );
		vglClCheckError( err, (char*) "clCreateProgramWithSource" );

		err = clBuildProgram( program, 1, cl.deviceId, NULL, NULL, NULL );
		vglClBuildDebug(err,program);
	}
	
	static cl_kernel kernel = NULL;
	if (kernel == NULL)
	{
		kernel = clCreateKernel( program, "threshold", &err ); 
		vglClCheckError( err, (char*) "clCreateKernel" );
	}

	err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void *) &src->oclPtr );
	vglClCheckError( err, (char*) "clSetKernelArg A" );
	err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void *) &dst->oclPtr );
	vglClCheckError( err, (char*) "clSetKernelArg B" );
	err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float *) &mobj_C );
	vglClCheckError( err, (char*) "clSetKernelArg C" );

	size_t worksize[] = { src->width, src->height, 1 };
	clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
	vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

	err = clReleaseMemObject( mobj_C );
	vglClCheckError(err, (char*) "clReleaseMemObject mobj_C");

        vglSetContext(dst, VGL_CL_CONTEXT);
}
void vglClConvolution(VglImage* src, VglImage* dst, float* convolution_window, int window_size_x, int window_size_y)
{
	cl_int err;
	
        vglCheckContext(src, VGL_CL_CONTEXT);
        vglCheckContext(dst, VGL_CL_CONTEXT);

	cl_mem mobj_C = NULL;
	mobj_C = clCreateBuffer(cl.context,CL_MEM_READ_ONLY,window_size_x*window_size_y*sizeof(float),NULL,&err);
	vglClCheckError( err, (char*) "clCreateBuffer convolution_window" );
	err = clEnqueueWriteBuffer(cl.commandQueue, mobj_C, CL_TRUE, 0, window_size_x*window_size_y*sizeof(float), convolution_window, 0, NULL, NULL);
	vglClCheckError( err, (char*) "clEnqueueWriteBuffer convolution_window" );

	cl_mem mobj_D = NULL;
	mobj_D = clCreateBuffer(cl.context,CL_MEM_READ_ONLY,sizeof(int),NULL,&err);
	vglClCheckError( err, (char*) "clCreateBuffer window_size_x" );
	err = clEnqueueWriteBuffer(cl.commandQueue, mobj_D, CL_TRUE, 0, sizeof(int), &window_size_x, 0, NULL, NULL);
	vglClCheckError( err, (char*) "clEnqueueWriteBuffer window_size_x" );

	cl_mem mobj_E = NULL;
	mobj_E = clCreateBuffer(cl.context,CL_MEM_READ_ONLY,sizeof(int),NULL,&err);
	vglClCheckError( err, (char*) "clCreateBuffer window_size_y" );
	err = clEnqueueWriteBuffer(cl.commandQueue, mobj_E, CL_TRUE, 0, sizeof(int), &window_size_y, 0, NULL, NULL);
	vglClCheckError( err, (char*) "clEnqueueWriteBuffer window_size_y" );

	static cl_program program = NULL;
	if (program == NULL)
	{
                char* file_path = (char*) "CL/vglClConvolution.cl";
		std::ifstream file(file_path);
		if(file.fail())
		{
			std::string str("File not found: ");
			str.append(file_path);
			vglClCheckError(-1,(char*)str.c_str());
		}
		std::string prog( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
		const char *source_str = prog.c_str();
#ifdef __DEBUG__
		printf("Kernel to be compiled:\n%s\n", source_str);
#endif

		program = clCreateProgramWithSource( cl.context, 1, (const char **) &source_str, 0, &err );
		vglClCheckError( err, (char*) "clCreateProgramWithSource" );

		err = clBuildProgram( program, 1, cl.deviceId, NULL, NULL, NULL );
		vglClBuildDebug(err,program);
	}
	
	static cl_kernel kernel = NULL;
	if (kernel == NULL)
	{
		kernel = clCreateKernel( program, "convolution", &err ); 
		vglClCheckError( err, (char*) "clCreateKernel" );
	}

	err = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void *) &src->oclPtr );
	vglClCheckError( err, (char*) "clSetKernelArg A" );
	err = clSetKernelArg( kernel, 1, sizeof( cl_mem ), (void *) &dst->oclPtr );
	vglClCheckError( err, (char*) "clSetKernelArg B" );
	err = clSetKernelArg( kernel, 2, sizeof( cl_mem ), (float *) &mobj_C );
	vglClCheckError( err, (char*) "clSetKernelArg C" );
	err = clSetKernelArg( kernel, 3, sizeof( cl_mem ), (int   *) &mobj_D );
	vglClCheckError( err, (char*) "clSetKernelArg D" );
	err = clSetKernelArg( kernel, 4, sizeof( cl_mem ), (int   *) &mobj_E );
	vglClCheckError( err, (char*) "clSetKernelArg E" );

	size_t worksize[] = { src->width, src->height, 1 };
	clEnqueueNDRangeKernel( cl.commandQueue, kernel, 2, NULL, worksize, 0, 0, 0, 0 );
	vglClCheckError( err, (char*) "clEnqueueNDRangeKernel" );

	err = clReleaseMemObject( mobj_C );
	vglClCheckError(err, (char*) "clReleaseMemObject mobj_C");
	err = clReleaseMemObject( mobj_D );
	vglClCheckError(err, (char*) "clReleaseMemObject mobj_D");
	err = clReleaseMemObject( mobj_E );
	vglClCheckError(err, (char*) "clReleaseMemObject mobj_E");

        vglSetContext(dst, VGL_CL_CONTEXT);
}


#endif
