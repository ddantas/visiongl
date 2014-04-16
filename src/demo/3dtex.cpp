#include <vglImage.h>
#include <glsl2cpp_shaders.h>

int main()
{
	vglInit(500,500);
	VglImage* img = vglCreateImage("../images/dicom/0003.dcm.%d.PGM",0,16,false);
        vglNot(img, img);
        vglDownload(img);
	vglSaveImage(img,"../images/3dtex_demo.%d.jpg",0,16);
	return 0;
}
