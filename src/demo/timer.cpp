/** \file timer.cpp

    Implements functions of libtimer.so

    Libtimer is a library for measuring the performance of functions in runtime. The time count is in microseconds.

  */

#define _CRT_SECURE_NO_WARNINGS

#include "timer.h"

//fps, clock
#ifdef __linux__
#include <sys/time.h>
#else
#include <Windows.h>
#endif

//malloc
#include <stdlib.h>
#include <stdio.h>

/** \brief Time elapsed.

    If called with no parameters, or a null parameter, returns the time elapsed, in microseconds, from the last call to TimerStart().

    If called with a non-zero parameter, works exaclty as TimerStart().
    
  */
#ifdef __linux__
long TimerElapsed(int start){

  static struct timeval *Tps = NULL;
  static struct timeval *Tpf = NULL;
  long retval;
  if (start || Tps == NULL){
    Tps = (struct timeval*) malloc(sizeof(struct timeval));
    gettimeofday (Tps, 0);
    Tpf = (struct timeval*) malloc(sizeof(struct timeval));
    gettimeofday (Tpf, 0);
  }
  else{
    gettimeofday (Tpf, 0);
  }
  retval = (Tpf->tv_sec-Tps->tv_sec)*1000000 + Tpf->tv_usec-Tps->tv_usec;
  return retval;
}
#else
double TimerElapsed(int start){
  /*static unsigned long Tps = -1;
  static unsigned long Tpf = -1;
  if (start || Tps == -1){
	  Tps = GetTickCount64();
	  Tpf = GetTickCount64();
  }
  else{
	  Tpf = GetTickCount64();
  }
  
  unsigned long retval = (Tpf - Tps);
  return retval;*/
  static LARGE_INTEGER *Tps = NULL;
  static LARGE_INTEGER *Tpf = NULL;
  static double freq = 0;
  if (start || Tps == NULL){
	  Tps = (LARGE_INTEGER*) malloc(sizeof(LARGE_INTEGER));
	  Tpf = (LARGE_INTEGER*) malloc(sizeof(LARGE_INTEGER));
	  QueryPerformanceFrequency((LARGE_INTEGER*)Tps);
	  freq = double(Tps->QuadPart/1000000.0f);

	  QueryPerformanceCounter(Tps);
	  QueryPerformanceCounter(Tpf);
  }
  else
  {
	  QueryPerformanceCounter(Tpf);
  }
  return double((Tpf->QuadPart - Tps->QuadPart)/freq);
}
#endif


/** \brief Timer start.

    Starts counting time in microseconds.
  */
void TimerStart(){
  TimerElapsed(1);
}

char* getTimeElapsed()
{
	char *ret = (char*) malloc(sizeof(char)*255);
#ifdef __linux__
	sprintf(ret,"%d us",TimerElapsed());
#else
	sprintf(ret,"%.2f us",TimerElapsed());
#endif
	return ret;
}

char* getTimeElapsedInSeconds()
{
	char *ret = (char*) malloc(sizeof(char)*255);
#ifdef __linux__
	sprintf(ret,"%.6f s",TimerElapsed()/1000000.0f);
#else
	sprintf(ret,"%.6f s",TimerElapsed()/1000000.0f);
#endif
	return ret;
}
