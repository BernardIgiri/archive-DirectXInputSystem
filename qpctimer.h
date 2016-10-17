/****************************************************************************
 qpctimer.h - Query Performance Timer formerly HiResTimer.h
 
 Wrapper for the high-resolution timer. Can't be used if the hi-res timer
 doesn't exist.
  
 Author   :   Dave Astle
 Date     :   2/1/2001

  Modified By:	Bernard Igiri
  Date		 :	11/27/2003

  - Multiple instances of this timer no longer results in conflict.

 Written for OpenGL Game Programming

  This code has the following dependancies.
  <windows.h> <float.h>


*****************************************************************************/

#ifndef __QPCTIMER_H_INCLUDED__
#define __QPCTIMER_H_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class CQPCTimer
{
public:
  CQPCTimer();
  ~CQPCTimer();
  bool Init();
  float GetElapsedSeconds(unsigned long elapsedFrames = 1);
  float GetFPS(unsigned long elapsedFrames = 1);
  float LockFPS(unsigned char targetFPS);
private:
  LARGE_INTEGER   m_startTime;
  LARGE_INTEGER   m_ticksPerSecond;
  LARGE_INTEGER	  m_lastTime1,
				  m_lastTime2,
				  m_lastTime3;
};

#endif // __QPCTIMER_H_INCLUDED__