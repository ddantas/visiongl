#include "vglImage.h"
#include "vglDcmtkIo.h"

#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "demo/timer.h"

#include <fstream>

float min(float a, float b)
{
  return (a < b) ? a : b;
}

float max(float a, float b)
{
  return (a > b) ? a : b;
}

float getColor(VglImage* image, int x, int y)
{ 
  x = (x > 0) ? x : 0;
  y = (y > 0) ? y : 0;
  x = (x < image->getWidth()-1) ? x : image->getWidth()-1;
  y = (y < image->getHeight()-1) ? y : image->getHeight()-1;
  unsigned char color = image->ipl->imageData[x + y*image->getWidth()];
  return ((float)color)/255.0f;
}

float getColor(VglImage* image, int x, int y, int z)
{
  x = max(min(x, 0), image->getWidth());
  y = max(min(y, 0), image->getHeight());
  z = max(min(z, 0), image->getLength());
  x = (x < image->getWidth() - 1) ? x : image->getWidth() - 1;
  y = (y < image->getHeight() - 1) ? y : image->getHeight() - 1;
  z = (z < image->getLength() - 1) ? z : image->getLength() - 1;
  unsigned char result = ((unsigned char*)image->ndarray)[x + y*image->getWidth() + z*image->getWidth() * image->getHeight()];
  /*if (result > 100)
  {
    printf("getColor value = %d\n", result);
  }*/
  return ((float)result)/255.0f;
}

void setColor(VglImage* image, int x, int y, float value)
{
  unsigned char result_value = value * 255;
  image->ipl->imageData[x + y*image->getWidth()] = result_value;
}

void setColor(VglImage* image, int x, int y, int z, float value)
{
  unsigned char result_value = value * 255;
  /*if (result_value > 100)
  {
    printf("setColor value = %d\n", result_value);
  }*/
  ((unsigned char*)image->ndarray)[x + y*image->getWidth() + z*image->getWidth() * image->getHeight()] = result_value;
}

enum ConvoluteType
{
  DILATE = 1,
  ERODE = 0
};

float MMD_STANDARD(float color, float mask_value, float value, ConvoluteType type, float gamma = 0.2f)
{
  if (type == ERODE)
  {
    float b = 1 - mask_value;
    float S = max(color, b);
    return min(value,S);
  }
  else
  {
    float b = mask_value;
    float S = min(color,b);
    return max(value,S);
  }
}

float MMD_ALGEBRAIC(float color, float mask_value, float value, ConvoluteType type, float gamma = 0.2f)
{
  if (type == ERODE)
  {
    float b = 1 - mask_value;
    float S = color + b - (color*b);
    return min(value, S);
  }
  else
  {
    float b = mask_value;
    float S = color*b;
    return max(value, S);
  }
}

float MMD_ARITHMETIC(float color, float mask_value, float value, ConvoluteType type, float gamma = 0.2f)
{
  if (type == ERODE)
  {
    float b = 1 - mask_value;
    float S = 1 - sqrt(min(1 - color, 1 - b)*(((1 - color) + (1 - b)) / 2));
    return min(value, S);
  }
  else
  {
    float b = mask_value;
    float S = sqrt(min(color, b)*((color + b) / 2));
    return max(value, S);
  }
}

float MMD_GEOMETRIC(float color, float mask_value, float value, ConvoluteType type, float gamma = 0.2f)
{
  if (type == ERODE)
  {
    float b = 1 - mask_value;
    float S = 1 - sqrt((1 - color)*(1 - b));
    return min(value, S);
  }
  else
  {
    float b = mask_value;
    float S = sqrt(color*b);
    return max(value, S);
  }
}

float MMD_BOUND(float color, float mask_value, float value, ConvoluteType type, float gamma = 0.2f)
{
  if (type == ERODE)
  {
    float b = 1 - mask_value;
    float S = min(1, color + b);
    return min(value, S);
  }
  else
  {
    float b = mask_value;
    float S = max(0, color + b - 1);
    return max(value, S);
  }
}

float MMD_DRASTIC(float color, float mask_value, float value, ConvoluteType type, float gamma = 0.2f)
{
  if (type == ERODE)
  {
    float b = 1 - mask_value;
    float S;
    if (b == 0)
      S = color;
    else if (color == 0)
      S = b;
    else
      S = 1;
    return min(value, S);
  }
  else
  {
    float b = mask_value;
    float S;
    if (b == 1)
      S = color;
    else if (color == 1)
      S = b;
    else
      S = 0;
    return max(value, S);
  }
}

float MMD_DaP(float color, float mask_value, float value, ConvoluteType type, float gamma = 0.2f)
{
  if (type == ERODE)
  {
    float b = 1 - mask_value;
    float S = 1 - (((1 - color)*(1 - b)) / (max(max(1 - color, 1 - b), gamma)));
    return min(value, S);
  }
  else
  {
    float b = mask_value;
    float S = (color*b) / (max(max(color, b), gamma));
    return max(value, S);
  }
}

float MMD_HAMACHER(float color, float mask_value, float value, ConvoluteType type, float gamma = 0.2f)
{
  if (type == ERODE)
  {
    float b = 1 - mask_value;
    float S = (color + b + ((gamma - 2)*color*b)) / (1 + ((gamma - 1)*color*b));
    return min(value, S);
  }
  else
  {
    float b = mask_value;
    float S = (color*b) / (gamma + ((1 - gamma)*(color + b - color*b)));
    return max(value, S);
  }
}

void CONVOLUTE(VglImage* image_in, VglImage* image_out, float* mask, int* mask_size, ConvoluteType type, float(*MMD_KERNEL)(float color, float mask_value, float value, ConvoluteType type, float gamma), float gamma = 0.2f)
{

  if (image_in->ndim == 2)
  {
    int factorx = floor((float)mask_size[0] / 2.0f);
    int factory = floor((float)mask_size[1] / 2.0f);
    for (int x = 0; x < image_in->getWidth(); x++)
    {
      for (int y = 0; y < image_in->getHeight(); y++)
      {
        float color = (type == DILATE) ? 0.0f : 1.0f;
        
        for (int mx = 0; mx < mask_size[0]; mx++)
        {
          for (int my = 0; my < mask_size[1]; my++)
          {
            float image_color = getColor(image_in, x + (mx - factorx), y + (my - factory));
            float mask_value = mask[mx + my*mask_size[0]];
            color = MMD_KERNEL(image_color, mask_value, color, type, gamma);
          }
        }
        setColor(image_out, x, y, color);
      }
    }
  }
  else if (image_in->ndim == 3)
  {
    int factorx = floor((float)mask_size[0] / 2.0f);
    int factory = floor((float)mask_size[1] / 2.0f);
    int factorz = floor((float)mask_size[2] / 2.0f);
    for (int x = 0; x < image_in->getWidth(); x++)
    {
      for (int y = 0; y < image_in->getHeight(); y++)
      {
        for (int z = 0; z < image_in->getLength(); z++)
        {
          float color = (type == DILATE) ? 0.0f : 1.0f;
          for (int mx = 0; mx < mask_size[0]; mx++)
          {
            for (int my = 0; my < mask_size[1]; my++)
            {
              for (int mz = 0; mz < mask_size[2]; mz++)
              {
                float image_color = getColor(image_in, x + (mx - factorx), y + (my - factory), z + (mz - factorz));
                float mask_value = mask[mx + my*mask_size[0] + mz*mask_size[0] * mask_size[1]];
                color = MMD_KERNEL(image_color, mask_value, color, type, gamma);
              }
            }
          }
          setColor(image_out, x, y, z, color);
        }
      }
    }
  }
}

void benchmark(VglImage* img_in, VglImage* img_out, float* mask, int* mask_size, int nSteps, ConvoluteType type, char* operator_name,char* prefix_path, float(*MMD_KERNEL)(float color, float mask_value, float value, ConvoluteType type, float gamma), float gamma = 0.2f)
{
  const float gama = 0.2f;
  TimerStart();
  CONVOLUTE(img_in, img_out, mask, mask_size, type, MMD_KERNEL, gama);
  if (img_in->ndim == 2)
    printf("First call to        %s %dx%d:                %s\n",operator_name,mask_size[0],mask_size[1], getTimeElapsedInSeconds());
  else if (img_in->ndim == 3)
    printf("First call to        %s %dx%dx%d:                %s\n", operator_name, mask_size[0], mask_size[1], mask_size[2], getTimeElapsedInSeconds());
  //Total time spent on n operations FuzzyErode 3x3
  int p = 0;
  TimerStart();
  while (p < nSteps)
  {
    p++;
    CONVOLUTE(img_in, img_out, mask, mask_size, type, MMD_KERNEL, gama);
    //vglClCopy(out,img);
  }
  if (img_in->ndim == 2)
    printf("Time spent on %8d %s %dx%d:                %s\n", nSteps, operator_name, mask_size[0], mask_size[1], getTimeElapsedInSeconds());
  else if (img_in->ndim == 3)
    printf("Time spent on %8d %s %dx%d%d:                %s\n", nSteps, operator_name, mask_size[0], mask_size[1], mask_size[2], getTimeElapsedInSeconds());

  char* outFilename = (char*)malloc(400);
  if (img_in->ndim == 2)
  {
    sprintf(outFilename, "%s/%s_%dx%d.%s", prefix_path, operator_name, mask_size[0], mask_size[1], "tif");
    cvSaveImage(outFilename, img_out->ipl);
  }
  else if (img_in->ndim == 3)
  {
    sprintf(outFilename, "%s/%s_%dx%dx%d.%s", prefix_path, operator_name, mask_size[0], mask_size[1], mask_size[2], "dcm");
    vglDcmtkSaveDicom(img_out, outFilename, 0);
    //vglGdcmSaveDicom(img_out, outFilename, 0);
  }
  
}

int main(int argc, char* argv[])
{
  if (argc != 4)
  {
    printf("\nUsage: MMDCPU_benchmark lena_1024.tiff 1000 /tmp\n\n");
    printf("In this example, will run the program for lena_1024.tiff in a \nloop with 1000 iterations. Output images will be stored in /tmp.\n\n");
    printf("Error: Bad number of arguments = %d. 3 arguments required.\n", argc - 1);
    exit(1);
  }
  //vglInit(50, 50);

  int nSteps = atoi(argv[2]);
  char* inFilename = argv[1];
  char* outPath = argv[3];
  char* outFilename = (char*)malloc(strlen(outPath) + 400);
  sprintf(outFilename, "%s%s", outPath, "/test_out.tif");

  printf("VisionGL-OpenCL on %s, %d operations\n\n", inFilename, nSteps);

  VglImage* img = vglLoadImage(inFilename, 0);
  if (img == NULL)
    img = vglDcmtkLoadDicom(inFilename);
  VglImage* out = vglCreateImage(img);
  float* mask;
  int* mask_size;
  if (img->ndim == 2)
  {
    mask = (float*)malloc(sizeof(float) * 9);
    float actual_mask[9] = { 0, 1, 0, 1, 1, 1, 0, 1, 0 };
    memcpy(mask, actual_mask, sizeof(float) * 9);

    mask_size = (int*)malloc(sizeof(int) * 2);
    int actual_mask_size[2] = { 3, 3 };
    memcpy(mask_size, actual_mask_size, sizeof(int) * 2);
  } 
  else
  {
    mask = (float*)malloc(sizeof(float) * 27);
    float actual_mask[27] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, };
    memcpy(mask, actual_mask, sizeof(float) * 27);

    mask_size = (int*)malloc(sizeof(int) * 3);
    int actual_mask_size[3] = { 3, 3, 3 };
    memcpy(mask_size, actual_mask_size, sizeof(int) * 3);
  }

  printf("Mask = { %d, %d, %d }\n", mask_size[0], mask_size[1], mask_size[2]);
  
  benchmark(img, out, mask, mask_size, nSteps, ERODE, "Standard Erode", outPath, &MMD_STANDARD);
  benchmark(img, out, mask, mask_size, nSteps, DILATE, "Standard Dilate", outPath, &MMD_STANDARD);

  benchmark(img, out, mask, mask_size, nSteps, ERODE, "Algebraic Erode", outPath, &MMD_ALGEBRAIC);
  benchmark(img, out, mask, mask_size, nSteps, DILATE, "Algebraic Dilate", outPath, &MMD_ALGEBRAIC);

  benchmark(img, out, mask, mask_size, nSteps, ERODE, "Arithmetic Erode", outPath, &MMD_ARITHMETIC);
  benchmark(img, out, mask, mask_size, nSteps, DILATE, "Arithmetic Dilate", outPath, &MMD_ARITHMETIC);

  benchmark(img, out, mask, mask_size, nSteps, ERODE, "Geometric Erode", outPath, &MMD_GEOMETRIC);
  benchmark(img, out, mask, mask_size, nSteps, DILATE, "Geometric Dilate", outPath, &MMD_GEOMETRIC);

  benchmark(img, out, mask, mask_size, nSteps, ERODE, "Bound Erode", outPath, &MMD_BOUND);
  benchmark(img, out, mask, mask_size, nSteps, DILATE, "Bound Dilate", outPath, &MMD_BOUND);

  benchmark(img, out, mask, mask_size, nSteps, ERODE, "Drastic Erode", outPath, &MMD_DRASTIC);
  benchmark(img, out, mask, mask_size, nSteps, DILATE, "Drastic Dilate", outPath, &MMD_DRASTIC);

  benchmark(img, out, mask, mask_size, nSteps, ERODE, "DaP Erode", outPath, &MMD_DaP);
  benchmark(img, out, mask, mask_size, nSteps, DILATE, "DaP Dilate", outPath, &MMD_DaP);

  benchmark(img, out, mask, mask_size, nSteps, ERODE, "Hamacher Erode", outPath, &MMD_HAMACHER);
  benchmark(img, out, mask, mask_size, nSteps, DILATE, "Hamacher Dilate", outPath, &MMD_HAMACHER);

  return 0;
}
