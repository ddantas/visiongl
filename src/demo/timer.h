/** \headerfile timer.h

    Functions prototypes of libtimer.so

    Include this file when using the functions of this library.

  */
#ifdef __linux__
long TimerElapsed(int start = 0);
#else
double TimerElapsed(int start = 0);
#endif

void TimerStart(void);

char* getTimeElapsed(void);
char* getTimeElapsedInSeconds();