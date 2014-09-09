

#include <CL/cl.h>
#include <CL/cl_gl.h>
#ifdef __linux__
    #include <GL/glx.h>
#endif

#include <stdio.h>
#include <string.h>

struct VglClContext
{
	cl_platform_id* platformId;
	cl_device_id* deviceId;
	cl_context context;
	cl_command_queue commandQueue;
};

VglClContext cl;
bool Interop;

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
    cl_device_type device_type = CL_DEVICE_TYPE_DEFAULT;
    err = clGetPlatformIDs(0, NULL, &num_platforms);
    vglClCheckError(err, (char*) "clGetPlatformIDs get number of platforms");
    cl.platformId = (cl_platform_id*)malloc(sizeof(cl_platform_id)*num_platforms);
    err = clGetPlatformIDs(num_platforms,cl.platformId,NULL);
    vglClCheckError(err, (char*) "clGetPlatformIDs get platforms id");	

    if (num_platforms == 0)
        printf("found no platform for opencl\n\n");
    else if (num_platforms >= 1)
        printf("found %d platform(s) for opencl\n\n", num_platforms);

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
    // To add CL_KHR_gl_sharing property to context, window id is needed.
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
			Interop = false;
		}
		search = strtok(NULL, " ");
	}
	
#ifdef __linux__
    cl_context_properties properties[] = {
      //CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
      //CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
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
    size_t val;
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &val, NULL);
    printf("%s: %s: CL_DEVICE_IMAGE3D_MAX_DEPTH: %ld px\n", __FILE__, __FUNCTION__, val);
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &val, NULL);
    printf("%s: %s: CL_DEVICE_IMAGE3D_MAX_HEIGHT: %ld px\n", __FILE__, __FUNCTION__, val);
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &val, NULL);
    printf("%s: %s: CL_DEVICE_IMAGE3D_MAX_WIDTH: %ld px\n", __FILE__, __FUNCTION__, val);
    err = clGetDeviceInfo(cl.deviceId[0], CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(size_t), &val, NULL);
    printf("%s: %s: CL_DEVICE_MAX_PARAMETER_SIZE: %ld mb\n", __FILE__, __FUNCTION__, val);

}

int main()
{

  vglClInit();


}
