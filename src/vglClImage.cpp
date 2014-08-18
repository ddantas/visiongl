
#ifdef __OPENCL__

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_shaders.h"


#include <fstream>

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
//#include <opencv2/core/core_c.h>
//#include <opencv2/highgui/highgui_c.h>


// cl-gl interoperability
#include <CL/cl_gl.h>
#ifdef __linux__
    #include <GL/glx.h>
#endif

VglClContext cl;
bool Interop = false;

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
        printf("Error %d while doing the following operation %s\n",error,name);
        //system("pause");
	exit(1);
    }
}

void vglClInit()
{
    cl_int err;
    cl_uint num_platforms, num_devices;
    cl_device_type device_type = CL_DEVICE_TYPE_CPU;
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

    err = clGetDeviceIDs(*cl.platformId, device_type, 0, NULL, &num_devices);
    vglClCheckError(err, (char*) "clGetDeviceIDs get number of devices");

    if (num_devices == 0)
    {
        printf("unable to find OpenCL devices, halting the program");
        exit(1);
    }
    else
        printf("found %d device(s)\n\n",num_devices);

    cl.deviceId = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
    err = clGetDeviceIDs(*cl.platformId, device_type, num_devices, cl.deviceId, NULL);
    vglClCheckError(err, (char*) "clGetDeviceIDs get devices id");
    //precisa adicionar a propriedade CL_KHR_gl_sharing no contexto e pra isso precisará do id do contexto do GL que deverá ser o parametro window
    //cl_context_properties props[] =	{CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext()};
	const int msgLen = 2048;
    char msg[msgLen];
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_EXTENSIONS, msgLen, msg, NULL);
	
	char* search = strtok ( msg, " ");
	bool found = false;
	while (search != NULL && !found)
	{
		if (strcmp(search, "cl_khr_gl_sharing") == 0)
		{
			printf("FOUND INTEROPERABILITY\n");
			Interop = true;
		}
		search = strtok(NULL, " ");
	}
	
#ifdef __linux__
    cl_context_properties properties[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
        CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
        CL_CONTEXT_PLATFORM, (cl_context_properties) cl.platformId[0], 
    0 };
#elif defined WIN32
    cl_context_properties properties[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties) cl.platformId[0],
    0 };
#endif

    cl.context = clCreateContext(properties,1,cl.deviceId,NULL, NULL, &err );
    //cl.context = clCreateContext(NULL,1,cl.deviceId,NULL, NULL, &err );
    vglClCheckError(err, (char*) "clCreateContext GPU");

    cl.commandQueue = clCreateCommandQueue( cl.context, *cl.deviceId, 0, &err );
    vglClCheckError( err, (char*) "clCreateCommandQueue" );

	err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_EXTENSIONS, msgLen, msg, NULL);
    printf("%s: %s: CL_DEVICE_EXTENSIONS:\n%s\n", __FILE__, __FUNCTION__, msg);
    cl_ulong vlong;
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &vlong, NULL);
    printf("%s: %s: CL_DEVICE_MAX_MEM_ALLOC_SIZE: %5.2f mb\n", __FILE__, __FUNCTION__, vlong/(1024.0f*1024));
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &vlong, NULL);
    printf("%s: %s: CL_DEVICE_GLOBAL_MEM_SIZE: %5.2f mb\n", __FILE__, __FUNCTION__, vlong/(1024.0f*1024));
    size_t img3d;
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &img3d, NULL);
    printf("%s: %s: CL_DEVICE_IMAGE3D_MAX_DEPTH: %d px\n", __FILE__, __FUNCTION__, img3d);
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &img3d, NULL);
    printf("%s: %s: CL_DEVICE_IMAGE3D_MAX_HEIGHT: %d px\n", __FILE__, __FUNCTION__, img3d);
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &img3d, NULL);
    printf("%s: %s: CL_DEVICE_IMAGE3D_MAX_WIDTH: %d px\n", __FILE__, __FUNCTION__, img3d);
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(size_t), &img3d, NULL);
    printf("%s: %s: CL_DEVICE_MAX_PARAMETER_SIZE: %d mb\n", __FILE__, __FUNCTION__, img3d);

}

void vglClFlush()
{
    cl_int err;
    err = clFlush( cl.commandQueue );
    vglClCheckError(err, (char*) "clFlush command_queue");
    err = clFinish( cl.commandQueue );
    vglClCheckError(err, (char*) "clFinish command_queue");
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
        //system("pause");
        exit(1);
    }
}


/** vglClUpload branch3d

 */
void vglClUpload(VglImage* img)
{
	if (Interop)
	{
		vglClUploadInterop(img);
	}
	else
	{
		if (img->nChannels == 3)
		{
			fprintf(stderr, "%s: %s: Error: image with 3 channels not supported. Please convert to 4 channels.\n", __FILE__, __FUNCTION__);
			exit(1);
		}
		if (img->nChannels == 3)
		{
			fprintf(stderr, "%s: %s: Error: ipl image field with 3 channels not supported. Please convert to 4 channels.\n", __FILE__, __FUNCTION__);
			exit(1);
		}
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

			cl_image_format format;
			if (img->nChannels == 1)
			{
				format.image_channel_order = CL_R;
				format.image_channel_data_type = CL_UNORM_INT8;
			}
			else if (img->nChannels == 4)
			{
		  /*
				if (img->iplRGBA == NULL)
				{
					printf("creating RGBA\n");
					img->iplRGBA = cvCreateImage(cvGetSize(img->ipl), IPL_DEPTH_8U, 4);
				}
		  */
				format.image_channel_order = CL_RGBA;
				format.image_channel_data_type = CL_UNORM_INT8;
			}

		if (img->ndim == 2)
		{
				img->oclPtr = clCreateImage2D(cl.context, CL_MEM_READ_WRITE, &format, img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], 0, NULL, &err);
				vglClCheckError( err, (char*) "clCreateImage2D" );
		}
		else if(img->ndim == 3)
		{
				img->oclPtr = clCreateImage3D(cl.context, CL_MEM_READ_WRITE, &format, img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], img->shape[VGL_LENGTH], 0, 0, NULL, &err);
				vglClCheckError( err, (char*) "clCreateImage3D" );
		}
		else
		{
				img->oclPtr = clCreateBuffer(cl.context, CL_MEM_READ_WRITE,img->getTotalSizeInBytes(),NULL,&err);
				vglClCheckError( err, (char*) "clCreateNDImage" );
		}

		/*
			cl_image_desc desc;

			if (img->ndim == 2)
			{
			
				desc.image_type = CL_MEM_OBJECT_IMAGE2D;
				desc.image_width = img->shape[VGL_WIDTH];
				desc.image_height = img->shape[VGL_HEIGHT];
				desc.image_depth = 0;
				desc.image_array_size = 1;
				desc.image_row_pitch = 0;
				desc.image_slice_pitch = 0;
				desc.num_mip_levels = 0;
				desc.num_samples = 0;
				desc.buffer = NULL;

			}
			else
			{
			
				desc.image_type = CL_MEM_OBJECT_IMAGE3D;
				desc.image_width = img->shape[VGL_WIDTH];
				desc.image_height = img->shape[VGL_HEIGHT];
				desc.image_depth = img->shape[VGL_LENGTH];
				desc.image_array_size = 0;
				desc.image_row_pitch = 0;
				desc.image_slice_pitch = 0;
				desc.num_mip_levels = 0;
				desc.num_samples = 0;
				desc.buffer = NULL;

			}
			img->oclPtr = clCreateImage(cl.context,CL_MEM_READ_WRITE, &format, &desc,NULL,&err);
			vglClCheckError(err, (char*) "clCreateImage");
		*/
		}

		if (vglIsInContext(img, VGL_RAM_CONTEXT))
		{
			size_t Origin[3] = { 0, 0, 0};

			int nFrames = 1;
			if(img->ndim == 3)
			{
				nFrames = img->shape[VGL_LENGTH];
			}

			void* imageData = NULL;
			if (img->ipl)
			{
				imageData = img->ipl->imageData;
			}
			else if (img->ndarray)
			{
				imageData = img->ndarray;
			}
			else
			{
				fprintf(stderr, "%s: %s: Error: both ipl and ndarray are NULL.\n", __FILE__, __FUNCTION__);
				exit(1);
			}
   
			if (img->ndim <=3)
			{
				size_t Size3d[3] = {img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], nFrames};
				err = clEnqueueWriteImage( cl.commandQueue, img->oclPtr, CL_TRUE, Origin, Size3d,0,0, (char*)imageData, 0, NULL, NULL );
				vglClCheckError( err, (char*) "clEnqueueWriteImage" );
				clFinish(cl.commandQueue);
			}
			else
			{
				err = clEnqueueWriteBuffer(cl.commandQueue, img->oclPtr, CL_TRUE, 0, img->getTotalSizeInBytes(), NULL, NULL, NULL, NULL);
			}
		}

		vglAddContext(img, VGL_CL_CONTEXT);
	}
}


void vglClUploadInterop(VglImage* img)
{
    vglUpload(img);
    vglGlToCl(img);
}


void vglClUploadForce(VglImage* img)
{
    vglSetContext(img,VGL_RAM_CONTEXT);
    vglUpload(img);
    vglGlToCl(img);
}

void vglClDownloadForce(VglImage* img)
{
    vglSetContext(img,VGL_CL_CONTEXT);
    vglClToGl(img);
    vglDownloadFaster(img);
}

void vglClDownloadInterop(VglImage* img)
{
    vglClToGl(img);
	if (vglIsInContext(img,VGL_GL_CONTEXT) && !vglIsInContext(img,VGL_RAM_CONTEXT))
	{
		if (img->ndim == 2)
			vglDownloadFaster(img);
		else
			vglDownload(img);
	}
}


void vglClAlloc(VglImage* img)
{
    glFlush();
    glFinish();

	cl_int err_cl;
    if (img->oclPtr == NULL)
    {
		if (img->ndim == 2)
		{
			img->oclPtr = clCreateFromGLTexture2D(cl.context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, img->tex, &err_cl);
			vglClCheckError(err_cl, (char*) "clCreateFromGLTexture");
		}
		else if(img->ndim == 3)
		{
			img->oclPtr = clCreateFromGLTexture3D(cl.context, CL_MEM_READ_WRITE, GL_TEXTURE_3D, 0, img->tex, &err_cl);
			vglClCheckError(err_cl, (char*) "clCreateFromGLTexture");
		}

    }
}


void vglGlToCl(VglImage* img)
{
    glFlush();
    glFinish();

    if (img->oclPtr == NULL)
    {
        vglClAlloc(img);
    }

    if (!vglIsInContext(img, VGL_CL_CONTEXT))
    {
        //printf("==========ACQUIRE: vgl = %p, ocl = %d\n", img, img->oclPtr);
		cl_int err_cl = clFlush(cl.commandQueue);
		vglClCheckError(err_cl, (char*) "clFlush");

		err_cl = clFinish(cl.commandQueue);
		vglClCheckError(err_cl, (char*) "clFinish");

        err_cl = clEnqueueAcquireGLObjects(cl.commandQueue, 1 , (cl_mem*) &img->oclPtr, 0 , NULL, NULL);
		vglClCheckError(err_cl, (char*) "clEnqueueAcquireGLObjects");
        
        vglSetContext(img, VGL_CL_CONTEXT);
    }
    //printf("Vai sair de %s\n", __FUNCTION__);
}

void vglClToGl(VglImage* img)
{
    //vglDownload(img);

    if (!vglIsInContext(img, VGL_CL_CONTEXT))
    {
        //vglGlToCl(img);      
        //fprintf(stderr, "vglClToGl: Error: image context = %d not in VGL_CL_CONTEXT\n", img->inContext);
        return;
	}

    cl_int err_cl;
    //printf("==========RELEASE: vgl = %p, ocl = %d\n", img, img->oclPtr);
    err_cl = clEnqueueReleaseGLObjects(cl.commandQueue, 1 , (cl_mem*) &img->oclPtr, 0 , NULL, NULL);
    vglClCheckError(err_cl, (char*) "clEnqueueReleaseGLObjects");

    err_cl = clFlush(cl.commandQueue);
    vglClCheckError(err_cl, (char*) "clFlush");

	err_cl = clFinish(cl.commandQueue);
	vglClCheckError(err_cl, (char*) "clFinish");

    vglSetContext(img, VGL_GL_CONTEXT);

    //printf("Vai sair de %s\n", __FUNCTION__);
    

}

void vglClDownload(VglImage* img)
{
	if (Interop)
	{
		vglClDownloadInterop(img);
	}
	else
	{
		if (img->nChannels == 3)
		{
			fprintf(stderr, "%s: %s: Error: ipl image field with 3 channels not supported. Please convert to 4 channels.\n", __FILE__, __FUNCTION__);
			exit(1);
		}

		if (!vglIsInContext(img, VGL_CL_CONTEXT))
		{
		  fprintf(stderr, "vglClDownload: Error: image context = %d not in VGL_CL_CONTEXT\n", img->inContext);
		  return;
		}

		size_t Origin[3] = { 0, 0, 0};

		if(img->ndim == 2)
		{
			size_t Size3d[3] = { img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], 1 };
			cl_int err_cl = clEnqueueReadImage( cl.commandQueue, img->oclPtr, CL_TRUE, Origin, Size3d, 0, 0, img->ipl->imageData, 0, NULL, NULL );
			vglClCheckError( err_cl, (char*) "clEnqueueReadImage2D" );

			//cvCvtColor(img->iplRGBA, img->ipl, CV_RGBA2BGR);
		}
		else if(img->ndim == 3)
		{
			size_t Size3d[3] = { img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], img->shape[VGL_LENGTH] };
			cl_int err_cl = clEnqueueReadImage( cl.commandQueue, img->oclPtr, CL_TRUE, Origin, Size3d, 0, 0,(char*) img->ndarray, 0, NULL, NULL );
			vglClCheckError( err_cl, (char*) "clEnqueueReadImage3D" );
		}
		else
		{
			cl_int err = clEnqueueReadBuffer(cl.commandQueue, img->oclPtr, CL_TRUE, 0, img->getTotalSizeInBytes(), img->ndarray, NULL, NULL, NULL);
			vglClCheckError( err, (char*) "clEnqueueReadNDImage" );
		}

		vglAddContext(img, VGL_RAM_CONTEXT);
	}
}


#endif





