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

find_library(NUMA_LIB numa)
if(NUMA_LIB)
    message("numa library found")
	set(NUMA numa) 
endif()
                    
                    

add_executable(benchfloat benchmark/float.c)
target_link_libraries(benchfloat ptrtimer ${NUMA})

add_executable(benchcast  benchmark/cast.c)
target_link_libraries(benchcast ptrtimer ${NUMA})

find_library(PTHREAD_LIB pthread)
if(PTHREAD_LIB)
    message("pthread library found")
    add_executable(benchfloatT benchmark/float-T.c)
    target_link_libraries(benchfloatT pthread ptrtimer)
else()
    message("pthread library not found")
endif()

find_library(FFTW3F_LIB fftw3f)
if(FFTW3F_LIB)
	message("fftw3f library found")
	add_executable(benchfftw3f benchmark/fftw3f.c)
	target_link_libraries(benchfftw3f fftw3f m ptrtimer ${NUMA})
else()
	message("fftw3f library not found")
endif()

find_library(FFTW3_LIB fftw3)
if(FFTW3_LIB)
	message("fftw3 library found")
	add_executable(benchfftw3 benchmark/fftw3.c)
	target_link_libraries(benchfftw3 fftw3 m ptrtimer ${NUMA})
else()
	message("fftw3 library not found")
endif()

find_library(MKL_LIB mkl)
if(MKL_LIB)
	message("MKL library found")
else()
	message("MKL library not found")
endif()

find_library(OPENCL_LIB OpenCL)
if(FFTW3_LIB)
	message("OpenCL library found")
	add_executable(benchopencl1 benchmark/opencl-1alloc.c)
	target_link_libraries(benchopencl1 OpenCL m ptrtimer ${NUMA})
	add_executable(benchopencl1B benchmark/opencl-1alloc-B.c)
	target_link_libraries(benchopencl1B OpenCL m ptrtimer ${NUMA})
	add_executable(benchopencl2 benchmark/opencl-2alloc.c)
	target_link_libraries(benchopencl2 OpenCL m ptrtimer ${NUMA})
else()
	message("OpenCL library not found")
endif()
