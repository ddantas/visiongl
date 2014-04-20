#include <vglImage.h>
#include <glsl2cpp_shaders.h>
#include <vglClImage.h>
#include <cl2cpp_shaders.h>

int main()
{
	vglInit(500,500);
	vglClInit();
	VglImage* img = vglCreateImage("../images/dicom/0003.dcm.%d.PGM",0,16,false);
	//3x3x3 mask for convolution
	float* mask333 = (float*) malloc(sizeof(float)*27);

	for(int i = 0; i < 27; i++)
		mask333[i] = 1.0f/27.0f;
	//5x5x5 mask for convolution
	float* mask555 = (float*) malloc(sizeof(float)*125);
	for(int i = 0; i < 125; i++)
		mask555[i] = 1.0f/125.0f;

	float blur333[] = {1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
						1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
						1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
						1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f,
						1.0f/16.0f, 1.0f/8.0f, 1.0f/16.0f, 
						1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
						1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f, 
						1.0f/32.0f, 1.0f/16.0f, 1.0f/32.0f, 
						1.0f/64.0f, 1.0f/32.0f, 1.0f/64.0f};
		
        //vglCl3dNot(img,img);
		//vglCl3dConvolution(img,img,mask555,5,5,5);
        vglClDownload(img);
	vglSaveImage(img,"../images/3dtex_demo.%d.jpg",0,16);
	clReleaseMemObject(img->oclPtr);
	vglClFlush();
	return 0;
}
