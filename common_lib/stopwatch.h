/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#ifndef ORIP_STOPWATCH_H
#define ORIP_STOPWATCH_H

namespace ORIP
{
class StopWatchImpl; // forward declaration

class StopWatch
{
private: 
  StopWatchImpl* m_Impl;

public:
  StopWatch();
  ~StopWatch();

  void start();
  void stop();
  void reset();

  //void stop_and_update();
  //void update();

  double get_time();
};

}

#endif // ORIP_STOPWATCH_H
