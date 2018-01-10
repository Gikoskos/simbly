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

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_REQUIRED_DEFINITIONS "-D=_GNU_SOURCE")
check_symbol_exists(strtok_r "string.h" HAVE_STRTOK_R)

if(${HAVE_STRTOK_R})

    check_symbol_exists(get_nprocs "sys/sysinfo.h" HAVE_GET_NPROCS)

    if(${HAVE_GET_NPROCS})

        include_directories(include libvoids/lib/include)

        add_executable(simbly ${SIMBLY_SRC} ${SIMBLY_INC} ${LIBVOIDS_INC})
        set_property(TARGET simbly PROPERTY C_STANDARD 99)

        target_link_libraries(simbly voids Threads::Threads)

        target_compile_options(simbly PRIVATE -Wall -Wextra -pedantic)

        target_compile_definitions(simbly PRIVATE "_GNU_SOURCE")

        if(CMAKE_BUILD_TYPE MATCHES Debug)
            target_compile_options(simbly BEFORE PRIVATE -g)
        else(CMAKE_BUILD_TYPE MATCHES Debug)
            target_compile_options(simbly BEFORE PRIVATE -O3)
        endif(CMAKE_BUILD_TYPE MATCHES Debug)

    else(${HAVE_GET_NPROCS})
        message(FATAL_ERROR "Couldn't find get_nprocs")
    endif(${HAVE_GET_NPROCS})

else(${HAVE_STRTOK_R})
    message(FATAL_ERROR "Couldn't find strtok_r")
endif(${HAVE_STRTOK_R})
