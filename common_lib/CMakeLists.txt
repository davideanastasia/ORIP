SET(CMN_LIB_HDRS
  commandlineparser.h
  common_lib.h
  roi_mask.h
  sleep_t.h
  timing.h

  matrix.h
  convolution.h
  framereader.h
  yuvreader.h
  stopwatch.h
  framewriter.h
  yuvwriter.h)

SET(CMN_LIB_SRCS
  commandlineparser.cpp
  common_lib.cpp

  #convolution.cpp
  framereader.cpp
  yuvreader.cpp
  stopwatch.cpp
  framewriter.cpp
  yuvwriter.cpp)

ADD_LIBRARY(common_lib STATIC ${CMN_LIB_SRCS} ${CMN_LIB_HDRS})
