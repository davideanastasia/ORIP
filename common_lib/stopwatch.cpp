/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#include "stopwatch.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN_TIMER_IMPL
#endif

#if defined(WIN_TIMER_IMPL)
#include <windows.h>
#else
#if defined(_MSC_VER)
#include <time.h> 
#include <Winsock2.h> 
#else
#include <sys/time.h> 
#endif
#endif

namespace ORIP
{
#if defined(WIN_TIMER_IMPL)

/*
* StopWatch Implementation: Windows version
*
*/

class StopWatchImpl
{
private: 
  LARGE_INTEGER m_start_t;
  LARGE_INTEGER m_stop_t;
  LARGE_INTEGER m_frequency;
  double m_elapsed_time;

public:
  StopWatchImpl(): m_elapsed_time(0.0)
  {
    QueryPerformanceFrequency(&m_frequency);
  }

  ~StopWatchImpl() {}

  void start()
  {
    QueryPerformanceCounter(&m_start_t);
  }

  void stop()
  {
    // stop timer
    QueryPerformanceCounter(&m_stop_t);
     
    // increment counter
    m_elapsed_time += ((double)((long)m_stop_t.QuadPart - (long)m_start_t.QuadPart));
  }

  void reset()
  {
    m_elapsed_time = 0.0;
  }

  double get_time()
  {
    return (m_elapsed_time * 1000.0 / m_frequency.QuadPart);
  }
};

#else

class StopWatchImpl
{
private: 
  timeval m_start_t;
  timeval m_end_t;
  double m_elapsed_time;

public:
  StopWatchImpl(): m_elapsed_time(0.0)
  {}

  ~StopWatchImpl()
  {}

  void start()
  {
    gettimeofday(&m_start_t, NULL);
    //start_t = clock();
  }

  void stop()
  {
    //end_t = clock();
    gettimeofday(&m_end_t, NULL);
    
    //__time = ((double)(end_t - start_t));
    double delta = (m_end_t.tv_sec - m_start_t.tv_sec) * 1000.0;    // sec to ms
    delta += (m_end_t.tv_usec - m_start_t.tv_usec) / 1000.0;        // us to ms

    m_elapsed_time += delta;
  }

  void reset()
  {
    m_elapsed_time = 0;
  }

  double get_time()
  {
    return m_elapsed_time;
  }
};

#endif

StopWatch::StopWatch():
m_Impl(new StopWatchImpl)
{}

StopWatch::~StopWatch()
{
  delete m_Impl;
}

void StopWatch::start()
{
  m_Impl->start();
}

void StopWatch::stop()
{
  m_Impl->stop();
}

void StopWatch::reset()
{
  m_Impl->reset();    
}

double StopWatch::get_time()
{
  return m_Impl->get_time();
}

} // namespace ORIP