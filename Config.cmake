



#################################
# CONFIG FILES
#################################
#Config file
#configure_file (
#  "${PROJECT_SOURCE_DIR}/include/config.hpp.in"
#  "${PROJECT_BINARY_DIR}/include/config.hpp"
#  )
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in"
  "${PROJECT_SOURCE_DIR}/include/config.h"
  )
