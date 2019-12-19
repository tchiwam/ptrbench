cmake_minimum_required (VERSION 2.6)
project (ptrbench)

message("compiler=${CMAKE_C_COMPILER_ID}")
message("compiler version=${CMAKE_C_COMPILER_VERSION}")
message("CFLAGS=$ENV{CFLAGS}")

set (ptrbench_VERSION_MAJOR "1")
set (ptrbench_VERSION_MINOR "0")
set (ptrbench_C_COMPILER_ID "\"${CMAKE_C_COMPILER_ID}\"")
set (ptrbench_C_COMPILER_VERSION "\"${CMAKE_C_COMPILER_VERSION}\"")
set (ptrbench_CFLAGS "\"$ENV{CFLAGS}\"")

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

if (DEFINED ENV{dir_base})
	message("dir_base is defined")
        include_directories("$ENV{dir_base}/include")
        link_directories("$ENV{dir_base}/lib")
else()
	     include_directories("${CMAKE_INSTALL_PREFIX}/include")
             link_directories("${CMAKE_INSTALL_PREFIX}/lib")
	     message("dir_base is not defined using CMAKE_INSTALL_PREFIX for headers")
endif()

include_directories("${PROJECT_BINARY_DIR}"
                    "${PROJECT_SOURCE_DIR}/include")

add_executable(benchfloat benchmark/float.c)
add_executable(benchcast  benchmark/cast.c)
target_link_libraries(benchfloat ptrtimer)
target_link_libraries(benchcast ptrtimer)

