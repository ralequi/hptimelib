cmake_minimum_required (VERSION 2.8.11)
project (High_Performance_Timing_Library)
# The version number.
execute_process(
    COMMAND git describe --always --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE hptl_VERSION)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories("include")

add_definitions(-std=gnu11 -Wall -Wextra -Werror -O3 -g -pipe -flto -ffat-lto-objects)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto -O3 -g -pipe")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto -O3 -g -pipe")

#Import all .hpp
file(GLOB lib_INCLUDE
    "include/*.h"
    "include/*.hpp"
)

#Import all .cpp
file(GLOB lib_SRC
    "src/*.c"
    "src/*.cpp"
)

include(Config.cmake)

#The library
# this is the "object library" target: compiles the sources only once
add_library(hptl_obj OBJECT ${lib_SRC} ${lib_INCLUDE})
# shared libraries need PIC
set_property(TARGET ${hptl_obj} PROPERTY POSITION_INDEPENDENT_CODE 1)
# actual shared and static libraries built from the same object files
add_library(hptl-shared SHARED $<TARGET_OBJECTS:hptl_obj>)
add_library(hptl STATIC $<TARGET_OBJECTS:hptl_obj>)

# Rename the lib
set_target_properties(hptl PROPERTIES
        OUTPUT_NAME hptl
        ARCHIVE_OUTPUT_NAME hptl)
set_target_properties(hptl-shared PROPERTIES
        OUTPUT_NAME hptl
        ARCHIVE_OUTPUT_NAME hptl)


#Library Dependencies
if(HPTL_CLOCKREALTIME)
    target_link_libraries(hptl-shared rt)
    target_link_libraries(hptl rt)
endif()

#Tests
add_executable(deviationTest src/test/deviationTest)
add_executable(pdeviationTest src/test/pdeviationTest.c)
add_executable(integrityTest src/test/integrityTest.c)
add_executable(exampleTest src/test/exampleTest)
add_executable(performanceTest src/test/performanceTest)

#Link library executables
target_link_libraries(deviationTest hptl)
target_link_libraries(pdeviationTest hptl)
target_link_libraries(integrityTest hptl)
target_link_libraries(exampleTest hptl)
target_link_libraries(performanceTest hptl)
