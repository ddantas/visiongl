
// visiongl
#include "glsl2cpp_shaders.h"
#include "vglImage.h"
#include "vglContext.h"

//time
#include <time.h>

void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

using namespace std;

#define   NUM_IMG         12
VglImage* img[NUM_IMG];

int width, height;

int i_frame = 0;
int disparity = 0;
int last_i_frame = -1;

char* winname[VGL_MAX_WINDOWS];
char* filename[VGL_MAX_WINDOWS];

VglNamedWindowList* window_list;

double julia_ox       = 0.0;
double julia_oy       = 0.0;

double julia_cx       = 0.0 * -1.36;
double julia_cy       = 0.0 * 0.11;

double julia_half_win = 1.0;
double julia_step     = 0.05;



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
    fprintf(stderr, "c = (%3.20f, %3.20f), halfwin = %3.20f\n", julia_ox, julia_oy, julia_half_win);
  } else {
    fpscounter++;
  }
  return fps;
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
      case 'r':
      case 'R':
         julia_ox = 0.0;
         julia_oy = 0.0;
         julia_half_win = 1.0;
         break;
      case '8':
         julia_oy -= julia_half_win*julia_step;
         disparity += 4;
         printf("Disparity = %d\n", disparity);
         break;
      case '2':
         julia_oy += julia_half_win*julia_step;
         disparity -= 4;
         printf("Disparity = %d\n", disparity);
         break;
      case '6':
         julia_ox += julia_half_win*julia_step;
	 i_frame++;
         break;
      case '4':
         julia_ox -= julia_half_win*julia_step;
	 i_frame--;
         break;
      case 'w':
         julia_oy -= julia_half_win*julia_step/4.0;
         break;
      case 's':
         julia_oy += julia_half_win*julia_step/4.0;
         break;
      case 'a':
         julia_ox += julia_half_win*julia_step/4.0;
         break;
      case 'd':
         julia_ox -= julia_half_win*julia_step/4.0;
         break;
      case 'z':
      case 'Z':
         julia_half_win *= .95;
         break;
      case 'x':
      case 'X':
	 julia_half_win /= .95;
         break;
      case 'q':
      case 'Q':
      case 27:
         glutLeaveMainLoop();
         break;
      default:
         break;
   }
}

void demo_fractal(void){
    vglMandel(img[0], julia_ox, julia_oy, julia_half_win);
    vglMandel(img[2], julia_ox, julia_oy, 1.0);
    vglJulia(img[1], 0.0, 0.0, julia_half_win, julia_ox, julia_oy);  
    vglJulia(img[3], 0.0, 0.0, 1.0, julia_ox, julia_oy);  

    window_list->RefreshAll(2);
    int fps = showstats();
}

void display_fractal(void){
    vglMandel(img[0], julia_ox, julia_oy, julia_half_win);
    vglJulia(img[1], 0.0, 0.0, julia_half_win, julia_ox, julia_oy);  

    window_list->RefreshAll(3);
    int fps = showstats();
}

void display_try(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(0.2,0.3,0.4);

}

int main(int argc, char** argv)
{
  int w = 1200;
  int h = 860;

  window_list  = new VglNamedWindowList();
  window_list->main_window_id = vglInit(1200, 860);

  glutDisplayFunc(display_fractal);
  glutIdleFunc(display_fractal);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  for (int i = 0; i < VGL_MAX_WINDOWS; i++){
    winname[i]  = (char*) malloc(255);
    filename[i] = (char*) malloc(255);
    sprintf(winname[i], "img[%d]", i);
  }

  //glewExperimental = GL_TRUE;
  

  img[0] = vglCreateImage(cvSize(w/3, h/3), IPL_DEPTH_32F, 3);
  vglClear(img[0], 0.0, 0.0, 0.0, 0.0);

  for (int i = 1; i < NUM_IMG; i++){
    img[i] = vglCreateImage(img[0]);
    if (!img[i]){
      fprintf(stderr, "Error creating img[%d]\n", i);
      exit(1);
    }
    vglClear(img[i], 0.0, 0.0, i * 0.1, 0.0);
    vglPrintImageInfo(img[i]);
    vglPrintContext(img[i], winname[i]);
  }

  for (int i = 0; i < NUM_IMG; i++){
    window_list->NamedWindow(winname[i]);
    window_list->ShowImage(winname[i], img[i]);
  }

  glutMainLoop();

  return 0;
}
