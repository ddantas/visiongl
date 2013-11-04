
//GL
#include <GL/glew.h>
#include <GL/freeglut.h>

// visiongl
#include "vglLoadShader.h"
#include "glsl2cpp_shaders.h"
#include "glsl2cpp_Stereo.h"
#include "glsl2cpp_DiffEq.h"
#include "vglImage.h"
#include "vglContext.h"
#include "vglCudaImage.h"
#include "vglSimpleBGModel.h"

//IplImage, cvLoadImage
#include <opencv2/highgui/highgui_c.h>

//time
#include <time.h>

//begin cameras
#define MAX_CAM 1
CvCapture *capture[MAX_CAM];
//end Cameras

void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

using namespace std;

#define   NUM_IMG         20
VglImage* img[NUM_IMG];
VglImage* img_gray[NUM_IMG];

GLuint tex = (unsigned int)-1;

int width, height;
int img_width, img_height;

int i_frame = 0;
int disparity = 0;
int last_i_frame = -1;


// indicates number of rows and columns of images in main window
int refreshall = 2;

char* winname[VGL_MAX_WINDOWS];

         
VglNamedWindowList* window_list;


int num_cam = 0;

float zoomfactor = 1.0;

int showstats() {
  static int lasttime;
  static int fpscounter; 
  static int fps;
  int curtime;
  curtime = time(NULL);
  if( lasttime != curtime) {
    fps=fpscounter;
    fpscounter=1;
    lasttime = curtime;
    fprintf(stderr, "fps = %d,  %f msecs\n",fps,1.0/((float)fps)*1000);
  } else {
    fpscounter++;
  }
  return fps;
}

void display_cam(void)
{

    for (int i = 0; i < num_cam; i++)
    {
        cvGrabFrame(capture[i]);
        cvCopy(cvRetrieveFrame(capture[i]), img[i]->ipl);
        vglSetContext(img[i], VGL_RAM_CONTEXT);
        vglUpload(img[i]);
    }
    i_frame++;
}


void display(void)
{

  display_cam();

  if (num_cam == 1)
  {
    vglCopy(img[0], img[1]);
  }

  vglCopy(img[0], img[2]);
  vglCopy(img[0], img[3]);
  vglCopy(img[0], img[4]);

  vglGray(img[0], img[2]);
  vglCopy(img[2], img[4]);

  vglDilateSq3(img[2], img[3]);
  vglDilateSq3(img[3], img[2]);

  vglDilateSq3(img[2], img[3]);
  vglDilateSq3(img[3], img[2]);

  vglDilateSq3(img[2], img[3]);
  vglDilateSq3(img[3], img[2]);

  vglSobelGradient(img[4], img[5]);
  vglDilateSq3(img[5], img[6]);

  vglDiff(img[4], img[5], img[7]);
  vglCopy(img[7], img[3]);

  window_list->RefreshAll(refreshall);

  //CHECK_FRAMEBUFFER_STATUS()
  //ERRCHECK()

  int fps = showstats();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   width = w, height = h;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 'q':
      case 'Q':
      case 27:
         glutLeaveMainLoop();
         break;
      default:
         break;
   }
}

void PrintCaptureProperty(CvCapture *capture){
  float result;
  IplImage *img_in = cvQueryFrame(capture);
  printf("CV_CAP_PROP_POS_MSEC %f\n", cvGetCaptureProperty(capture, CV_CAP_PROP_POS_MSEC));
  printf("CV_CAP_PROP_POS_FRAMES %f\n", cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES));
  printf("CV_CAP_PROP_POS_AVI_RATIO %f\n", cvGetCaptureProperty(capture, CV_CAP_PROP_POS_AVI_RATIO));

  result = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
  if (result == 0.0) result = img_in->width;
  printf("CV_CAP_PROP_FRAME_WIDTH %f\n", result);

  result = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
  if (result == 0.0) result = img_in->height;
  printf("CV_CAP_PROP_FRAME_HEIGHT %f\n", result);

  printf("CV_CAP_PROP_FPS %f\n", cvGetCaptureProperty(capture, CV_CAP_PROP_FPS));
  printf("CV_CAP_PROP_FOURCC %f\n", cvGetCaptureProperty(capture, CV_CAP_PROP_FOURCC));
}

int main(int argc, char** argv)
{
  for (int i = 0; i < MAX_CAM; i++){
    capture[i] = 0;
    capture[i] = cvCaptureFromCAM(i);
    cvSetCaptureProperty(capture[i], CV_CAP_PROP_FPS, 30.0f);
    if (capture[i]){
      printf("Camera %d at %p\n", i, capture[i]);
      PrintCaptureProperty(capture[i]);
      num_cam++;
      printf("\n\n");
    }
   }

  window_list  = new VglNamedWindowList();
  window_list->main_window_id = vglInit(1280, 960);

  glutDisplayFunc(display);
  glutIdleFunc(display);

  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);


  for (int i = 0; i < VGL_MAX_WINDOWS; i++){
    winname[i]  = (char*) malloc(255);
    sprintf(winname[i], "win[%d]", i);
  }

  for (int i = 0; i < num_cam; i++){
    const int HAS_MIPMAP = 0;
    img[i]      = vglCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3, 1, HAS_MIPMAP);
    img_gray[i] = vglCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1, 1, HAS_MIPMAP);

    if (!img[i]){
      fprintf(stderr, "Error loading img[%d] from file\n", i);
      exit(1);
    }
    //vglPrintImageInfo(img[i]);
  }

  for (int i = num_cam; i < NUM_IMG; i++){
    img[i] = vglCreateImage(img[0]);
    img_gray[i] = vglCreateImage(img_gray[0]);
    if (!img[i]){
      fprintf(stderr, "Error creating img[%d]\n", i);
      exit(1);
    }
    vglClear(img[i], 0.0, 0.0, 0.0, 0.0);
    //vglPrintImageInfo(img[i]);
    char* img_name = (char*)malloc(255);
    sprintf(img_name, "img[%d]", i);    
    vglPrintContext(img[i], img_name);
  }

  for (int i = 0; i < 4; i++){
    window_list->NamedWindow(winname[i]);
    window_list->ShowImage(winname[i], img[i]);
  }

  GLint iUnits;
  glGetIntegerv(GL_MAX_TEXTURE_UNITS, &iUnits);
  printf("Max number of textures = %d\n", iUnits);
  
  glutMainLoop();

  return 0;
}
