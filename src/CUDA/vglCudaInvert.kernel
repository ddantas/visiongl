
/** vglCudaInvert

    Inverts image stored in cuda context.
*/

// <<<input->getWidth(),384>>> (IN_PBO: VglImage* input, OUT_PBO: VglImage* output)
// (input->cudaPtr, output->cudaPtr, input->getWidth(), input->getHeight(), input->nChannels)

template<typename T> 
__global__ void global_Invert(T* input, T* output, int w, int h, int nChannels){
    int offset = blockIdx.x * nChannels * w;
    T* array_in  = input  + offset;
    T* array_out = output + offset;
    for (int j = threadIdx.x; j < nChannels * w; j += blockDim.x){
      array_out[j] = -array_in[j];
    }
}
