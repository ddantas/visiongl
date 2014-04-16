#include <vglImage.h>

int main()
{
	vglInit(500,500);
	VglImage* img = vglCreateImage("../images/medical images/0003.dcm.%d.PGM",0,16,false);
	vglSaveImage(img,"../images/3dtex_demo.%d.jpg",0,16);
	return 0;
}