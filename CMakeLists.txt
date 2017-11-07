cmake_minimum_required (VERSION 2.6)
project (ptrbench)

set (ptrtimer_VERSION_MAJOR 1)
set (ptrtimer_VERSION_MINOR 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

if (DEFINED ENV{dir_base})
	message("dir_base is defined")
        include_directories("$ENV{dir_base}/include")
        link_directories("$ENV{dir_base}/lib")
else()
	message("dir_base is not defined")
endif()

include_directories("${PROJECT_BINARY_DIR}"
                    "${PROJECT_SOURCE_DIR}/include")

add_executable(benchfloat benchmark/float.c)
add_executable(benchcast  benchmark/cast.c)
target_link_libraries(benchfloat ptrtimer)
target_link_libraries(benchcast ptrtimer)

