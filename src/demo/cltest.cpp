

#include <CL/cl.h>
#include <CL/cl_gl.h>
#ifdef __linux__
    #include <GL/glx.h>
#endif

#include <stdio.h>


struct VglClContext
{
	cl_platform_id* platformId;
	cl_device_id* deviceId;
	cl_context context;
	cl_command_queue commandQueue;
};

VglClContext cl;

void vglClPrintContext(void)
{
  printf("cl_platform_id* platformId    = %p\n", cl.platformId);
  printf("cl_device_id* deviceId        = %p\n", cl.deviceId);
  printf("cl_context context            = %p\n", cl.context);
  printf("cl_command_queue commandQueue = %p\n", cl.commandQueue);
}

void vglClPrintPlatformInfo(int i)
{
  printf("%s: %s start\n", __FILE__, __FUNCTION__);
  cl_int err;
  int msg_len = 1024;
  char message[msg_len];
  size_t param_value_size = msg_len;
  size_t param_value_size_ret;

  printf("message = %s\n", message);

  err = clGetPlatformInfo(cl.platformId[i], CL_PLATFORM_PROFILE, param_value_size, message, &param_value_size_ret);
  
  printf("cl_platform_id[%d].profile = %s\n", i, message);

  printf("%s: %s end\n", __FILE__, __FUNCTION__);
}

void vglClCheckError(cl_int error, char* name)
{
    if (error != CL_SUCCESS)
    {
        printf("Erro %d while doing the following operation: %s\n",error,name);
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

	err = clGetDeviceIDs(*cl.platformId,CL_DEVICE_TYPE_CPU,0,NULL,&num_devices);
	vglClCheckError(err, (char*) "clGetDeviceIDs get number of devices");
	cl.deviceId = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
	err = clGetDeviceIDs(*cl.platformId,CL_DEVICE_TYPE_CPU,num_devices,cl.deviceId,NULL);
	vglClCheckError(err, (char*) "clGetDeviceIDs get devices id");
	//precisa adicionar a propriedade CL_KHR_gl_sharing no contexto e pra isso precisará do id do contexto do GL que deverá ser o parametro window
	//cl_context_properties props[] =	{CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext()};


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

}

int main()
{

  vglClInit();


}
