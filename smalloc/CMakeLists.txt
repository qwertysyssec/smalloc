cmake_minimum_required(VERSION 3.0)
project(smalloc)

enable_testing()


include_directories(include)


add_subdirectory( src build/src )
add_subdirectory( test build/test )
