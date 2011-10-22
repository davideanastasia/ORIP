LINK_DIRECTORIES(${ORIP_BINARY_DIR}/common_lib)
INCLUDE_DIRECTORIES(${ORIP_SOURCE_DIR}/common_lib)

ADD_EXECUTABLE(conv_std_d main.cpp library.cpp library.h)
TARGET_LINK_LIBRARIES(conv_std_d common_lib) 
