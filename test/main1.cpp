#include <list>
#include <array>
#include <algorithm>
#include <cassert>
#include "../include/manager.hpp"
using BigArray = std::array< unsigned char, 5000>;

void TestFunc(){
	std::list<BigArray, smalloc::allocator<BigArray> > v;
	const std::list<BigArray, smalloc::allocator<BigArray> > expected = {BigArray{'a'}, BigArray{'b'}, BigArray{'c'},};
	v.push_back(BigArray{'a'});
	v.push_back(BigArray{'b'});
	v.push_back(BigArray{'F'});
	v.push_back(BigArray{'c'});
	v.erase(std::next(v.begin(), 2));
	assert(v == expected);

}



int main(){
	TestFunc();
	return 0;

}
