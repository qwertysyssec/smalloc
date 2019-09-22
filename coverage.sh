sudo rm default.profdata
sudo rm default.profraw
clang++ main.cpp headers.cpp smalloc.cpp -o manager -Wall -pedantic-errors -std=c++17 -fprofile-instr-generate -fcoverage-mapping
./manager
llvm-profdata merge default.profraw -o default.profdata
llvm-cov report manager -instr-profile=default.profdata
llvm-cov show manager manager -instr-profile=default.profdata
