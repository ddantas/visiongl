/*********************************************************************
***                                                                ***
***  Header file vglClImage.h                                      ***
***                                                                ***
***  ddantas, hdanilo 21/1/2014                                    ***
***                                                                ***
*********************************************************************/

#ifndef __VGLCLIMAGE_H__
#define __VGLCLIMAGE_H__


#ifdef __OPENCL__

#include <CL/cl.h>

#include "vglImage.h"

struct VglClContext
{
	cl_platform_id* platformId;
	cl_device_id* deviceId;
	cl_context context;
	cl_command_queue commandQueue;
};

void vglClPrintContext(void);
void vglClCheckError(cl_int error, char* name);
void vglClInit();
void vglClFlush();
void vglClBuildDebug(cl_int err, cl_program program);
void vglClUploadForce(VglImage* img);
void vglClDownloadForce(VglImage* img);
void vglClUpload(VglImage* img);
void vglClDownload(VglImage* img);
void vglClUploadInterop(VglImage* img);
void vglClDownloadInterop(VglImage* img);
void vglClToGl(VglImage* img);
void vglGlToCl(VglImage* img);
void vglClAlloc(VglImage* img);

#endif

#endif
