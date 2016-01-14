


/** vglCudaInvertOnPlace

    Inverts image, stored in cuda context, on place.
*/

// <<<input->getHeight(),384>>> (IO_PBO: VglImage* input)
// (input->cudaPtr, input->getWidth(), input->getHeight(), input->nChannels)

  template<typename T> 
  __global__ void global_InvertOnPlace(T* input, int w, int h, int nChannels){
    T* array = input + blockIdx.x * nChannels * w;
    for (int j = threadIdx.x; j < nChannels * w; j += blockDim.x){
      array[j] = -array[j];
    }
  }
