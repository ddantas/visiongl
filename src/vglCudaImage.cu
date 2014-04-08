
//fprintf
#include <iostream>

#include "vglCudaImage.h"
#include "vglContext.h"
//#include "vglImage.h"

//cuda
//#include <cutil.h>
#include <cuda_gl_interop.h>

/** vglCudaAllocPbo

    Allocate PBO (Pixel Buffer Object) memory before
    transfering data to cuda context.

    Returns 1 in case of success and 0 in case of failure.

*/
int vglCudaAllocPbo(VglImage* img){
  int dbg = 0;

  if (dbg) printf("vglCudaAllocPbo: started, cudaPbo = %d.\n", img->cudaPbo);
  if (img->cudaPbo == (unsigned int)-1){
    if (dbg) printf("vglCudaAllocPbo: will allocate cudaPbo.\n");
    glGenBuffers(1, &img->cudaPbo);
    if (!img->cudaPbo){
      fprintf(stderr, "vglCudaAllocPbo: Error: unable to generate PBO in file '%s' in line %i.\n",
                __FILE__, __LINE__); 
      return 0;
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, img->cudaPbo);
    ERRCHECK()

    if (dbg) printf("vglCudaAllocPbo: allocated cudaPbo = %d\n", img->cudaPbo);

    glBufferData(GL_PIXEL_PACK_BUFFER, img->shape[VGL_WIDTH] * img->shape[VGL_HEIGHT] * img->nChannels, 0, GL_STREAM_DRAW);
    ERRCHECK()
  }
  else{
    glBindBuffer(GL_PIXEL_PACK_BUFFER, img->cudaPbo);
    ERRCHECK()
  }
  return 1;
}

/** vglCudaFreePbo

    Release PBO (Pixel Buffer Object) memory after 
    reading data from cuda context.

    Returns 1 in case of success and 0 in case of failure.

*/
int vglCudaFreePbo(VglImage* img){
  if (img->cudaPbo != (unsigned int)-1){
    glDeleteBuffers(1, &(img->cudaPbo));
    ERRCHECK()
  }
  return 1;
}

/** vglCudaMapPbo

    Map PBO (Pixel Buffer Object) to cuda context 
    after vglCudaAllocPbo

    Returns 1 in case of success and 0 in case of failure.

*/
int vglCudaMapPbo(VglImage* img){
  cudaError_t cudaErr;
  int dbg = 0;

  vglCudaAllocPbo(img);

  if (img->cudaPtr == NULL){
    cudaErr = cudaGLRegisterBufferObject(img->cudaPbo); 
    if (dbg) printf("chk215 cudaError = %d\n", cudaErr);
    if (cudaErr != cudaSuccess){
      fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",
              __FILE__, __LINE__, cudaGetErrorString( cudaErr) ); 
      return 0;
    }

    cudaErr = cudaGLMapBufferObject((void**)&(img->cudaPtr), img->cudaPbo);
    if (dbg) printf("chk220 cudaError = %d\n", cudaErr);
    if (cudaErr != cudaSuccess){
      fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",
              __FILE__, __LINE__, cudaGetErrorString( cudaErr) ); 
      return 0;
    }
  }
  return 1;
}

/** vglCudaUnmapPbo

    Unmap PBO (Pixel Buffer Object) from cuda context
    before using OpenGL.

    Returns 1 in case of success and 0 in case of failure.

*/
int vglCudaUnmapPbo(VglImage* img){
  cudaError_t cudaErr;
  int dbg = 0;

  if (img->cudaPtr != NULL){
    cudaErr = cudaGLUnmapBufferObject(img->cudaPbo);
    if (dbg) printf("chk225 cudaError = %d\n", cudaErr);
    if (cudaErr != cudaSuccess){
      fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",
              __FILE__, __LINE__, cudaGetErrorString( cudaErr) ); 
      return 0;
    }

    cudaErr = cudaGLUnregisterBufferObject(img->cudaPbo);
    if (dbg) printf("chk230 cudaError = %d\n", cudaErr);
    if (cudaErr != cudaSuccess){
      fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",
              __FILE__, __LINE__, cudaGetErrorString( cudaErr) ); 
      return 0;
    }

    img->cudaPtr = NULL;
  }

  return 1;
}

/** vglCudaAlloc

    Allocate PBO (Pixel Buffer Object) memory if needed.

    Then maps PBO to cuda context. Almost the same as vglGlToCuda
    but without calling glReadPixels

    Returns 1 in case of success and 0 in case of failure.

*/
int vglCudaAlloc(VglImage* img){

  if (!vglCudaAllocPbo(img))
    return 0;

  if (!vglCudaMapPbo(img))
    return 0;

  return 1;
}

/** vglCudaFree

    Unmaps PBO (Pixel Buffer Object) from cuda context.

    Then releases PBO memory. Almost the same as vglCudaToGl
    but without calling glTexImage2D to copy pixels.

    Returns 1 in case of success and 0 in case of failure.

*/
int vglCudaFree(VglImage* img){

  if (!vglCudaAllocPbo(img))
    return 0;

  if (!vglCudaMapPbo(img))
    return 0;

  return 1;
}

/** vglGlToCuda

    Allocate PBO (Pixel Buffer Object) memory if needed.

    Then send image data from texture memory to cuda context.

    Returns 1 in case of success and 0 in case of failure.

    Time < .5ms
*/
int vglGlToCuda(VglImage* img){
  int dbg = 0;

  if (dbg) printf("vglGlToCuda: started, cudaPbo = %d.\n", img->cudaPbo);

  vglCudaUnmapPbo(img);

  int ok = vglCudaAllocPbo(img);
  ERRCHECK()

  if (dbg) printf("vglGlToCuda: vglCudaAllocPbo returned ok = %d\n", ok);
  if (dbg) printf("vglGlToCuda: img->cudaPbo = %d\n", img->cudaPbo);

  if (dbg) printf("vglGlToCuda: vgl = %p\n", img);
  if (dbg) printf("vglGlToCuda: vgl w x h: %dx%d\n", img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT]);
  // FBO -> PBO (2ms)
  // glBufferData(w*h*3) <-> glReadPixels(GL_RGB)
  // glBufferData(w*h*4) <-> glReadPixels(GL_RGBA)
  // If not done like this, will give "OpenGL Error: invalid operation" 
  //    after glReadPixels
  //GLvoid* lixo = (GLvoid*)malloc(640*480*4);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, img->fbo);
  ERRCHECK()
  glReadPixels(0, 0, img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) 0);
  ERRCHECK()
  //SavePPM("/tmp/lixo.ppm", 640, 480, lixo);

  // Tex -> PBO (4ms)
  /*
  glBindTexture(GL_TEXTURE_2D, img->tex);
  ERRCHECK()
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) 0);
  ERRCHECK()
  */
  // end

  vglCudaMapPbo(img);

  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  ERRCHECK()
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  ERRCHECK()

  vglAddContext(img, VGL_CUDA_CONTEXT);

  return 1;
}

/** vglCudaToGl

    Then get image data from cuda context to texture memory.

    Returns 1 in case of success and 0 in case of failure.

    Time < .5ms
*/
int vglCudaToGl(VglImage* img){
  int dbg = 0;
 
  if (dbg)
    printf("vglCudaToGl: started, cudaPbo = %d.\n", img->cudaPbo);


  vglCudaUnmapPbo(img);

  /**/ // PBO -> Tex (.2ms)
  glBindTexture(GL_TEXTURE_2D, img->tex);
  ERRCHECK()
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, img->cudaPbo);
  ERRCHECK()
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->shape[VGL_WIDTH], img->shape[VGL_HEIGHT], 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) 0);
  ERRCHECK()
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  ERRCHECK()
  glBindTexture(GL_TEXTURE_2D, 0);
  ERRCHECK()
  /**/

  vglSetContext(img, VGL_GL_CONTEXT);

  return 1;
}




