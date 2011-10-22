/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */

#include <omp.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>

class Sleep_T
{
private:
  LARGE_INTEGER sleep_t[2];
  LARGE_INTEGER freq_t;

  int _t;
  double sleep;

public:
  Sleep_T()
  {
    QueryPerformanceFrequency(&freq_t);

    QueryPerformanceCounter(&sleep_t[0]);
    QueryPerformanceCounter(&sleep_t[1]);
  }

  void set_and_go( int t )
  {
    _t = t; // set sleeping time

    QueryPerformanceCounter(&sleep_t[0]);
    QueryPerformanceCounter(&sleep_t[1]);

#pragma omp critical
    {
      sleep = (double)(sleep_t[1].QuadPart - sleep_t[0].QuadPart);
    }

    while ( sleep*1000/freq_t.QuadPart <= _t )
    {
#pragma omp critical
      {
        QueryPerformanceCounter(&sleep_t[1]);
        sleep = (double)(sleep_t[1].QuadPart - sleep_t[0].QuadPart);
      }
    }
    //cout << "_" <<sleep*1000/freq_t.QuadPart<<"_";
  }

  void restart()
  {
    QueryPerformanceCounter(&sleep_t[0]);
  }

  void stop()
  {
    _t = 0;
  }

};

#else

#include <sys/time.h> 

class Sleep_T
{
private:
  timeval sleep_t[2];

  int _t;
  double sleep;

public:
  Sleep_T()
  {
    gettimeofday(&sleep_t[0], NULL);
    gettimeofday(&sleep_t[1], NULL);
  }

  void set_and_go( int t )
  {
    _t = t; // set sleeping time

    gettimeofday(&sleep_t[0], NULL);
    gettimeofday(&sleep_t[1], NULL);

#pragma omp critical
    {
      sleep = (sleep_t[1].tv_sec - sleep_t[0].tv_sec) * 1000.0;      // sec to ms
      sleep += (sleep_t[1].tv_usec - sleep_t[0].tv_usec) / 1000.0;   // us to ms
    }

    while ( sleep <= _t )
    {
#pragma omp critical
      {
        gettimeofday(&sleep_t[1], NULL);

        sleep = (sleep_t[1].tv_sec - sleep_t[0].tv_sec) * 1000.0;      // sec to ms
        sleep += (sleep_t[1].tv_usec - sleep_t[0].tv_usec) / 1000.0;   // us to ms
      }
    }
    //cout << "_" << sleep <<"_";
  }

  void restart()
  {
    gettimeofday(&sleep_t[0], NULL);
  }

  void stop()
  {
    _t = 0;
  }
};

#endif

