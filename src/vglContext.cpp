

//fprintf, stderr
#include <stdio.h>
//printf, stdout
#include <iostream>

#ifdef __CUDA__
#include "vglCudaImage.h"
#endif

#ifdef __OPENCL__
#include "vglClImage.h"
#endif

#include "vglContext.h"
#include "vglImage.h"

#define DEBUG_VGLCONTEXT 0

//#define vglIsContextUnique(x) ( (x>=1) && (x<=4) && (x!=3) )
//#define vglIsInContext(img, x) ( (img)->inContext & (x) )

/*  Call after copy. Context must be unique.

    Return 0 in case of error. Resulting context if successful.
 */
int vglAddContext(VglImage* img, int context){
  if (!vglIsContextUnique(context)){
    fprintf(stderr, "vglAddContext: Error: context = %d is not unique or invalid\n", context);
    return 0;
  }
  img->inContext = img->inContext | context;
  return img->inContext;
}

/*  Call after processing or creating image. Context must be unique. 
    After creating, use VGL_BLANK_CONTEXT    

    Return 0 in case of error. Resulting context if successful.
 */
int vglSetContext(VglImage* img, int context){
  if (!vglIsContextUnique(context) && context != 0){
    fprintf(stderr, "vglSetContext: Error: context = %d is not unique\n", context);
    return 0;
  }
  #if DEBUG_VGLCONTEXT
  printf("vglSetContext: context = %d\n", context);
  #endif
  img->inContext = context;
  return img->inContext;
}

/*  Call before using image. Context must be unique.

    Return 0 in case of error. Resulting context if successful.

    Shaders (vglCopy, vglDilateSq3, vglCudaInvertOnPlace etc)
    CALL ->
    vglCheckContext
    CALL ->
    vglUpload, vglDownload, vglGlToCuda, vglCudaToGl
    CALL ->
    vglSetContext, vglAddContext

 */
int vglCheckContext(VglImage* img, int context){
  if (!vglIsContextUnique(context)){
    fprintf(stderr, "vglCheckContext: Error: context = %d is not unique or invalid\n", context);
    return 0;
  }
  if (vglIsInContext(img, context)){
    #if DEBUG_VGLCONTEXT
    printf("vglCheckContext: image already in context %d\n", context);
    #endif
    return context;
  }
  switch (context){
    case VGL_RAM_CONTEXT:
      if (vglIsInContext(img, VGL_BLANK_CONTEXT)){
        //printf("%s: case 1\n", __FUNCTION__);
        vglAddContext(img, VGL_RAM_CONTEXT);
      }
#ifdef __OPENCL__
      else
      if (vglIsInContext(img, VGL_CL_CONTEXT)){
        //printf("%s: case 2\n", __FUNCTION__);
        vglClDownload(img);
      }
#endif
      else
      if (!vglIsInContext(img, VGL_RAM_CONTEXT))
      {
        //printf("%s: case 3\n", __FUNCTION__);
        vglCheckContext(img, VGL_GL_CONTEXT);
        if (vglIsInContext(img, VGL_GL_CONTEXT)){
          vglDownload(img);
        }
      }
      else{
        fprintf(stderr, "vglCheckContext: Internal Error: unable to transfer to RAM from invalid context = %d\n", img->inContext);
      }
    break;
    case VGL_GL_CONTEXT:
      if (vglIsInContext(img, VGL_BLANK_CONTEXT)){
        //printf("%s: case 1\n", __FUNCTION__);
        vglAddContext(img, VGL_RAM_CONTEXT);
      }
#ifdef __OPENCL__
      else
      if (vglIsInContext(img, VGL_CL_CONTEXT)){
        vglClDownload(img);        
        //if (!ok){
        //  fprintf(stderr, "vglCheckContext: error transfering from cuda to gl\n");
	//}
      }
#endif
      if (vglIsInContext(img, VGL_RAM_CONTEXT)){
        vglUpload(img);
      }
#ifdef __CUDA__
      else if (vglIsInContext(img, VGL_CUDA_CONTEXT)){
        int ok = vglCudaToGl(img);
        if (!ok){
          fprintf(stderr, "vglCheckContext: error transfering from cuda to gl\n");
	}
      }
#endif
      else{
        fprintf(stderr, "vglCheckContext: Internal Error: unable to transfer to GL from invalid context = %d\n", img->inContext);
        vglPrintImageInfo(img);
      }
    break;
#ifdef __CUDA__
    case VGL_CUDA_CONTEXT:
      vglCheckContext(img, VGL_GL_CONTEXT);
      #if DEBUG_VGLCONTEXT
      printf("vglCheckContext: will transfer from gl to cuda\n");
      #endif
      if (vglIsInContext(img, VGL_GL_CONTEXT)){
        int ok = vglGlToCuda(img);
        if (!ok){
          fprintf(stderr, "vglCheckContext: Error: unable to transfer from gl to cuda\n");
	}
      }
      else{
        fprintf(stderr, "vglCheckContext: Internal Error: unable to transfer to CUDA from invalid context = %d\n", img->inContext);
      }
    break;
#endif
#ifdef __OPENCL__
    case VGL_CL_CONTEXT:
      if (vglIsInContext(img, VGL_BLANK_CONTEXT)){
        vglClUpload(img);
      }
      else{
        vglCheckContext(img, VGL_RAM_CONTEXT);
        #if DEBUG_VGLCONTEXT
        printf("vglCheckContext: will transfer from ram to cl\n");
        #endif
        if (vglIsInContext(img, VGL_RAM_CONTEXT) || vglIsInContext(img, VGL_BLANK_CONTEXT)){
          vglClUpload(img);
          //if (!ok){
          //  fprintf(stderr, "vglCheckContext: Error: unable to transfer from ram to cl\n");  
         //}
        }
        else{
          fprintf(stderr, "vglCheckContext: Internal Error: unable to transfer to CL from invalid context = %d\n", img->inContext);
        }
      }
    break;
#endif
    default:
      fprintf(stderr, "vglCheckContext: Error: Trying to copy to invalid context = %d\n", context);
      return 0;
  }
  return img->inContext;
}

/*  Call before writing to image. Context must be unique.

    GL and RAM images are always allocated. CUDA images must be allocated
    if cudaPbo == -1.

 */
int vglCheckContextForOutput(VglImage* img, int context){
  #if DEBUG_VGLCONTEXT
  printf("vglCheckContextForOutput: started\n");
  #endif
  if (img){
    #if DEBUG_VGLCONTEXT
    printf("vglCheckContextForOutput: img not null\n");
    #endif
    if (context == VGL_CUDA_CONTEXT){
      #if DEBUG_VGLCONTEXT
      printf("vglCheckContextForOutput: context is cuda, pbo = %d ptr = %x\n", img->cudaPbo, img->cudaPtr);
      #endif
      if (img->cudaPbo == -1 || img->cudaPtr == 0){
        #if DEBUG_VGLCONTEXT
        printf("vglCheckContextForOutput: pbo == -1 so will allocate\n");
        #endif
        #ifdef __CUDA__
        return vglCudaAlloc(img);
        #else
        return 0;
        #endif
      }
    }
  }
}


void vglPrintContext(int context, char* msg){
  if (msg) printf("%s", msg);
  printf("(");
  ( context & VGL_RAM_CONTEXT ? printf("RAM ") : printf("    ") );
  ( context & VGL_GL_CONTEXT ? printf("GL ") : printf("   ") );
  ( context & VGL_CUDA_CONTEXT ? printf("CUDA") : printf("    ") );
  printf(") (%d)\n", context);
}

void vglPrintContext(VglImage* img, char* msg){
  vglPrintContext(img->inContext, msg);
}



