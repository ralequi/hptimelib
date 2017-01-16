#################################
# HPTL CONFIG
#################################

option(HPTL_DEBUG "Enable HPTL debug text" ON)
option(HPTL_ONLYLINUXAPI "Disables custom HPTL functionality and compiles as a Linux-Time wrapper" OFF)
option(HPTL_128b "Compiles in 128b mode" ON)
option(HPTL_128b_mixed "Compiles in 128b mode, but without (almost) fill the high part" ON)

#set   (HPTL_DEBUG "https" CACHE STRING "The server api URI-protocol")

#check RDTSC (only Intel)
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64") 
  option(HPTL_TSC "Enable TSC" ON)
endif()

include(CheckLibraryExists)
CHECK_LIBRARY_EXISTS(rt clock_gettime "time.h" HPTL_CLOCKREALTIME)

#################################
# CONFIG FILES
#################################
#Config file
#configure_file (
#  "${PROJECT_SOURCE_DIR}/include/config.hpp.in"
#  "${PROJECT_BINARY_DIR}/include/config.hpp"
#  )
configure_file (
  "${PROJECT_SOURCE_DIR}/include/hptl_config.h.in"
  "${PROJECT_SOURCE_DIR}/include/hptl_config.h"
  )
