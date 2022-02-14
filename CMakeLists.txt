cmake_minimum_required(VERSION 3.16)

project(osLinux)
set(CMAKE_C_STANDARD 11)


set(
        SOURCE_FILES
        main.c
        util.h
        util.c
)
add_executable(osLinux ${SOURCE_FILES})

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREADS_FLAG TRUE)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")


target_link_libraries(osLinux ${CMAKE_THREADS_LIB_INIT})