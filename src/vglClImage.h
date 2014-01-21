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


void vglClCheckError(cl_int error, char* name);
void vglClInit();
void vglClFlush();
void vglClBuildDebug(cl_int err, cl_program program);
void vglClUpload(VglImage* src);
void vglClUpload(VglImage* src, bool force_copy);
void vglClCopy(VglImage* src, VglImage* dst, bool copyToRam);
void vglClCopy(VglImage* src, VglImage* dst);
void vglClInvert(VglImage* src, VglImage* dst, bool copyToRam);
void vglClInvert(VglImage* src, VglImage* dst);
void vglClThreshold(VglImage* src, VglImage* dst, float thresh, bool copyToRam);
void vglClThreshold(VglImage* src, VglImage* dst, float thresh);
void vglClConvolution(VglImage* src, VglImage* dst, float* convolution_window, int window_size_x, int window_size_y, bool copyToRam);
void vglClConvolution(VglImage* src, VglImage* dst, float* convolution_window, int window_size_x, int window_size_y);
void vglClDownload(VglImage* img);

#endif

#endif
