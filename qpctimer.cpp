#include "qpctimer.h"
#include <float.h>

/*****************************************************************************
  CQPCTimer Class Constructor
*****************************************************************************/
CQPCTimer::CQPCTimer()
{
}
/*****************************************************************************
  CQPCTimer Class Destroyer
*****************************************************************************/
CQPCTimer::~CQPCTimer()
{
}
/*****************************************************************************
 Init()

 If the hi-res timer is present, the tick rate is stored and the function
 returns true. Otherwise, the function returns false, and the timer should
 not be used.
*****************************************************************************/
bool CQPCTimer::Init()
{
  if (!QueryPerformanceFrequency(&m_ticksPerSecond))
  {
    // system doesn't support hi-res timer
    return false;
  }
  else
  {
    QueryPerformanceCounter(&m_startTime);
	m_lastTime1=m_lastTime2=m_lastTime3=m_startTime;
    return true;
  }
} // end Init()


float CQPCTimer::GetElapsedSeconds(unsigned long elapsedFrames)
{
  LARGE_INTEGER currentTime;

  QueryPerformanceCounter(&currentTime);

  register float seconds =  ((float)currentTime.QuadPart - (float)m_lastTime1.QuadPart) / (float)m_ticksPerSecond.QuadPart;
  if (seconds==0.0f)
	  seconds+=FLT_MIN;


  // reset the timer
  m_lastTime1 = currentTime;

  return seconds;
} // end GetElapsedSeconds()


/***************************************************************************
 GetFPS()

 Returns the average frames per second over elapsedFrames, which defaults to
 one. If this is not called every frame, the client should track the number
 of frames itself, and reset the value after this is called.
***************************************************************************/
float CQPCTimer::GetFPS(unsigned long elapsedFrames)
{
  LARGE_INTEGER currentTime;

  QueryPerformanceCounter(&currentTime);

  float fps = (float)elapsedFrames * (float)m_ticksPerSecond.QuadPart / ((float)currentTime.QuadPart - (float)m_lastTime2.QuadPart);

  // reset the timer
  m_lastTime2 = currentTime;

  return fps;
} // end GetFPS


/***************************************************************************
 LockFPS()

 Used to lock the frame rate to a set amount. This will block until enough
 time has passed to ensure that the fps won't go over the requested amount.
 Note that this can only keep the fps from going above the specified level;
 it can still drop below it. It is assumed that if used, this function will
 be called every frame. The value returned is the instantaneous fps, which
 will be <= targetFPS.
***************************************************************************/
float CQPCTimer::LockFPS(unsigned char targetFPS)
{
  if (targetFPS == 0)
    targetFPS = 1;

  LARGE_INTEGER currentTime;
  float   fps;

  // delay to maintain a constant frame rate
  do {
    QueryPerformanceCounter(&currentTime);
    fps = (float)m_ticksPerSecond.QuadPart/((float)(currentTime.QuadPart - m_lastTime3.QuadPart));
  } while (fps > (float)targetFPS);

  // reset the timer
  m_lastTime3 = currentTime;//m_startTime;

  return fps;
} // end LockFPS()
