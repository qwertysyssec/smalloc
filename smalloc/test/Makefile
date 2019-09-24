CPPFLAGS=-Wall -g -pedantic-errors -fsanitize=address,undefined -std=c++17
NEEDEDCPP= /home/pasha/Documents/git_stuff/smalloc/smalloc/src/headers.cpp



test1: main.cpp ../src/headers.cpp ../include/manager.hpp  
	clang++ ${CPPFLAGS} ${NEEDEDCPP}  main.cpp -o test1 
test2: main1.cpp ../include/headers.hpp ../src/headers.cpp
	clang++ ${CPPFLAGS} ${NEEDEDCPP} main1.cpp -o test2
	




