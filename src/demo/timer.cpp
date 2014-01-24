/** \file timer.cpp

    Implements functions of libtimer.so

    Libtimer is a library for measuring the performance of functions in runtime. The time count is in microseconds.

  */

#include "timer.h"

//fps, clock
#include <sys/time.h>

//malloc
#include <stdlib.h>

/** \brief Time elapsed.

    If called with no parameters, or a null parameter, returns the time elapsed, in microseconds, from the last call to TimerStart().

    If called with a non-zero parameter, works exaclty as TimerStart().
    
  */
long TimerElapsed(int start){
  static struct timeval *Tps = NULL;
  static struct timeval *Tpf = NULL;
  long retval;

  if (start or Tps == NULL){
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

/** \brief Timer start.

    Starts counting time in microseconds.
  */
void TimerStart(){
  TimerElapsed(1);
}
