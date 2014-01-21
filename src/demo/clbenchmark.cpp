

#include "vglClImage.h"
#include "vglContext.h"

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include <time.h>

int main()
{
	
	printf("X\n");
	vglInit(50,50);
	printf("X\n");
	vglClInit();

	printf("X\n");

	char* image_path = "../images/lena_std.tif";
	VglImage* img = vglLoadImage(image_path,1,0);

        vglClUpload(img);
        cvSet(img->ipl, CV_RGB(255,0,0));
        cvSaveImage("../images/lena_conv33_red.tif", img->ipl);

        vglClDownload(img);
        cvSaveImage("../images/lena_conv33_download.tif", img->ipl);




	if (img == NULL)
	{
                printf("Y\n");
		std::string str("cvLoadImage/File not found: ");
                printf("Y\n");
		str.append(image_path);
                printf("Y\n");
		vglClCheckError(-1,(char*) str.c_str());
                printf("Y\n");
	}
	VglImage* out = vglCreateImage(img);
	float convolution[3][3] = { {1.0f/13.0f,2.0f/13.0f,1.0f/13.0f},{2.0f/13.0f,1.0f/13.0f,2.0f/13.0f},{1.0f/13.0f,2.0f/13.0f,1.0f/13.0f} }; //Operador blur
	clock_t t1, t2;
	t1 = clock();
	printf("X 100\n");
	vglClConvolution(img, out, *convolution, 3, 3);
	printf("X 200\n");
	t2 = clock();
	printf("Primeira chamada da vglClConvolution: %.2f s \n", (t2-t1)/(double)CLOCKS_PER_SEC);



	printf("X 300\n");
	printf("CLBENCHMARH CHECKCONTEXT\n");
        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_conv33_out.tif", out->ipl);


        vglClDownload(out);
        cvSaveImage("../images/lena_conv33_out2.tif", out->ipl);


	printf("X 400\n");
	printf("CLBENCHMARH CHECKCONTEXT\n");
        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_conv33_img.tif", img->ipl);
	printf("X 500\n");

        exit(0);


	//Mede o tempo para 1000 convoluções 3x3 sem a criação da operação
	int p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		vglClConvolution(img,out,*convolution,3,3,false);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 convolucoes 3x3: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);


        //vglCheckContext(out, VGL_RAM_CONTEXT);
        //cvSaveImage("../images/lena_conv33_out.tif", out->ipl);
        //vglCheckContext(img, VGL_RAM_CONTEXT);
        //cvSaveImage("../images/lena_conv33_img.tif", img->ipl);



	//Mede o tempo para 1000 convoluções 5x5 sem a criação da operação
	float convolution2[5][5] = { {1.0f/29.0f,2.0f/29.0f,3.0f/29.0f,2.0f/29.0f,1.0f/29.0f},
								 {3.0f/29.0f,2.0f/29.0f,1.0f/29.0f,2.0f/29.0f,3.0f/29.0f},
								 {1.0f/29.0f,2.0f/29.0f,3.0f/29.0f,2.0f/29.0f,1.0f/29.0f} };
	p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		vglClConvolution(img,out,*convolution,5,5,false);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 convolucoes 5x5: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_conv55.tif", out->ipl);

	//Mede o tempo para 1000 thresholds sem a criação da operação
	t1 = clock();
	vglClThreshold(img,out,0.5f);
	t2 = clock();
	printf("Primeira chamada da vglClThreshold: %.2f s \n", (t2-t1)/(double)CLOCKS_PER_SEC);
	p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		vglClThreshold(out,out,0.5f,false);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 threshold: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_thresh.tif", out->ipl);

	//Mede o tempo para 1000 thresholds sem a criação da operação
	t1 = clock();
	vglClInvert(img,out);
	t2 = clock();
	printf("Primeira chamada da vglClInvert: %.2f s \n", (t2-t1)/(double)CLOCKS_PER_SEC);
	p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		vglClInvert(out,out,false);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 invert: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);
	VglImage* gray = vglCreateImage(img);
	gray->ipl = cvCreateImage(cvGetSize(gray->ipl),IPL_DEPTH_8U,1);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_invert.tif", out->ipl);

	//Mede o tempo para 1000 conversão RGB->Grayscale na CPU
	p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		cvCvtColor(img->ipl,gray->ipl, CV_BGR2GRAY);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 conversões BGR->Gray: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);


        vglCheckContext(gray, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_rgbtogray.tif", gray->ipl);

	//Mede o tempo para 1000 conversão RGB->RGBA na CPU
	p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		cvCvtColor(img->ipl,out->iplRGBA, CV_BGR2RGBA);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 conversões BGR->RGBA: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_rgbtorgba.tif", out->iplRGBA);

	//Mede o tempo para 1000 copia CPU->GPU
	p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		vglClUpload(img,true);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 copia CPU->GPU, inclui conversão BGR->RGBA: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);


        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_upload.tif", img->ipl);

	//Mede o tempo para 1000 copia GPU->CPU
	p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		vglClDownload(img);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 copia GPU->CPU inclui conversão RGBA->BGR: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);


        vglCheckContext(img, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_download.tif", img->ipl);

	//Mede o tempo para 1000 copia GPU->GPU
	t1 = clock();
	vglClCopy(img,out);
	t2 = clock();
	printf("Primeira chamada da vglClCopy: %.2f s \n", (t2-t1)/(double)CLOCKS_PER_SEC);

	p = 0;
	t1 = clock();
	while (p < 1000)
	{
		p++;
		vglClCopy(img,out,false);
	}
	t2 = clock();
	printf("Tempo gasto para fazer 1000 copia GPU->GPU: %.2f s\n", (t2-t1)/(double)CLOCKS_PER_SEC);


        vglCheckContext(out, VGL_RAM_CONTEXT);
        cvSaveImage("../images/lena_clcopy.tif", out->ipl);

	//flush
	vglClFlush();
	system("pause");
	return 0;
}
