#include <vglImage.h>
#include <glsl2cpp_shaders.h>
#include <vglClImage.h>
#include <cl2cpp_shaders.h>

int main()
{
	vglInit(500,500);
	vglClInit();
	VglImage* img = vglCreateImage("../images/dicom/0003.dcm.%d.PGM",0,16,false);
        vglCl3dNot(img,img);
        vglClDownload(img);
	vglSaveImage(img,"../images/3dtex_demo.%d.jpg",0,16);
	return 0;
}
