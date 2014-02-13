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
#include <CL/cl_gl.h>

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
void vglClUpload(VglImage* src);
void vglClUpload(VglImage* src, bool force_copy);
void vglClDownload(VglImage* img);

#endif

#endif
