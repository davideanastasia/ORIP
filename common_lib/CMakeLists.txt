SET(CMN_LIB_HDRS
  commandlineparser.h
  common_lib.h 
  roi_mask.h
  sleep_t.h
  timing.h
  matrix.h)

SET(CMN_LIB_SRCS
  commandlineparser.cpp
  common_lib.cpp)

ADD_LIBRARY(common_lib STATIC ${CMN_LIB_SRCS} ${CMN_LIB_HDRS})