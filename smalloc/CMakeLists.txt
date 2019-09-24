cmake_minimum_required(VERSION 2.8)

project(smalloc)
add_executable(${PROJECT_NAME} "main.cpp" "smalloc.hpp" "smalloc.cpp" "smalloc_random.hpp" "manager.hpp" "headers.hpp" "headers.cpp")

#add_subdirectory(test build/test)
