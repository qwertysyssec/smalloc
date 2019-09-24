cmake_minimum_required(VERSION 3.0)

project(test)
add_executable(test_one "main.cpp")
add_executable(test_two "main1.cpp")

target_link_libraries(test_one smalloc)
target_link_libraries(test_two smalloc) 

add_test(NAME Test1 COMMAND test_one)
add_test(NAME Test2 COMMAND test_two)

