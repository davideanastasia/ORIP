/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */

// TIMER -----
#include <iostream>

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>

class My_Timer {

private: 
  LARGE_INTEGER *lp_perf_counter;
  LARGE_INTEGER frequency;
  double wrk_time;

public:
  My_Timer()
  {
    wrk_time = 0.0;
    lp_perf_counter = new LARGE_INTEGER[2]; //(LARGE_INTEGER *)calloc(2, sizeof(LARGE_INTEGER));
    QueryPerformanceFrequency(&frequency);

    std::cout << " Setup Windows Timer (" << frequency.QuadPart << ")"<< std::endl;
  }

  ~My_Timer()
  {
    delete lp_perf_counter;
  }

  inline void start()
  {
    QueryPerformanceCounter(lp_perf_counter);
  }

  inline void stop()
  {
    QueryPerformanceCounter(lp_perf_counter+1);
  }

  inline void update()
  {
    wrk_time += ((double)((long)lp_perf_counter[1].QuadPart - (long)lp_perf_counter[0].QuadPart));
  }

  inline void stop_and_update()
  {
    QueryPerformanceCounter(lp_perf_counter + 1);                                                          // ---- stop timer!
    wrk_time += double( (long)lp_perf_counter[1].QuadPart - (long)lp_perf_counter[0].QuadPart );    // increment counter
  }

  inline void reset()
  {
    wrk_time = 0.0;
  }

  inline double get_time()
  {
    return (wrk_time * 1000.0 / frequency.QuadPart);
  }
};

#else
// #include <ctime>
#include <sys/time.h> 

class My_Timer {

private: 
  //clock_t start_t, end_t;
  timeval start_t, end_t;
  double __time;

public:
  My_Timer()
  {
    std::cout << " Setup C-Like Timer" << std::endl;
  }

  ~My_Timer()
  {

  }

  inline void start()
  {
    gettimeofday(&start_t, NULL);
    //start_t = clock();
  }

  inline void stop()
  {
    gettimeofday(&end_t, NULL);
    //end_t = clock();
  }

  inline void update()
  {
    //__time = ((double)(end_t - start_t));
    double delta = 0;

    delta = (end_t.tv_sec - start_t.tv_sec) * 1000.0;      // sec to ms
    delta += (end_t.tv_usec - start_t.tv_usec) / 1000.0;   // us to ms

    __time += delta;
  }

  inline void stop_and_update()
  {
    double delta = 0;

    //end_t = clock();
    gettimeofday(&end_t, NULL);

    delta = (end_t.tv_sec - start_t.tv_sec) * 1000.0;      // sec to ms
    delta += (end_t.tv_usec - start_t.tv_usec) / 1000.0;   // us to ms

    __time += delta;
  }

  inline void reset()
  {
    __time = 0;
  }

  inline double get_time()
  {
    return __time;
  }
};

#endif
