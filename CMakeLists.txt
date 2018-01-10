cmake_minimum_required(VERSION 3.0.2)

project(simbly C)

add_subdirectory(libvoids)

set(SIMBLY_SRC src//error.c
               src//exec.c
               src//global.c
               src//program.c
               src//runtime.c
               src//scanner.c
               src//main.c)

set(SIMBLY_INC src//error.h
               src//exec.h
               src//global.h
               src//program.h
               src//runtime.h
               src//scanner.h
               src//common.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(include libvoids/lib/include)

add_executable(simbly ${SIMBLY_SRC} ${SIMBLY_INC} ${LIBVOIDS_INC})
set_property(TARGET simbly PROPERTY C_STANDARD 99)

target_link_libraries(simbly voids Threads::Threads)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(voids PRIVATE -Wall -Wextra -pedantic)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_options(voids BEFORE PRIVATE -g)
    endif(CMAKE_BUILD_TYPE MATCHES Debug)
endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
