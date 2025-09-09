cmake_minimum_required (VERSION 3.15)
project (High_Performance_Timing_Library LANGUAGES C CXX)
# The version number.
execute_process(
    COMMAND git describe --always --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE hptl_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

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

# Optionally exclude legacy wrappers from the build
if(NOT HPTL_ENABLE_LEGACY)
    list(FILTER lib_SRC EXCLUDE REGEX ".*/hptl_deprecated\\.c$")
endif()

#The library
# this is the "object library" target: compiles the sources only once
add_library(hptl_obj OBJECT ${lib_SRC} ${lib_INCLUDE})
# compile options and features
target_compile_features(hptl_obj PUBLIC c_std_11)
target_compile_options(hptl_obj PRIVATE -Wall -Wextra -Werror -O3 -pipe)
# shared libraries need PIC
set_property(TARGET hptl_obj PROPERTY POSITION_INDEPENDENT_CODE ON)
# actual shared and static libraries built from the same object files
add_library(hptl-shared SHARED $<TARGET_OBJECTS:hptl_obj>)
add_library(hptl STATIC $<TARGET_OBJECTS:hptl_obj>)

# Try enabling LTO/IPO if supported
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set_property(TARGET hptl-shared PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET hptl PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_include_directories(hptl_obj PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(hptl-shared PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(hptl PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

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
add_executable(deviationTest src/test/deviationTest.c)
add_executable(pdeviationTest src/test/pdeviationTest.c)
add_executable(integrityTest src/test/integrityTest.c)
add_executable(exampleTest src/test/exampleTest.c)
add_executable(performanceTest src/test/performanceTest.cpp)

#Link library executables
target_link_libraries(deviationTest hptl)
target_link_libraries(pdeviationTest hptl)
target_link_libraries(integrityTest hptl)
target_link_libraries(exampleTest hptl)
target_link_libraries(performanceTest hptl)

# Ensure tests are built with similar optimizations and IPO as the library
set(test_targets deviationTest pdeviationTest integrityTest exampleTest performanceTest)
foreach(t ${test_targets})
  target_compile_options(${t} PRIVATE -O3 -g -pipe)
  if(ipo_supported)
    set_property(TARGET ${t} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endforeach()
