#include <iostream>
#include <cassert>
#include "../include/manager.hpp"
#include <vector>
#include  <cassert>
#include <list>

using BigArray = std::array<unsigned char, 300>;


bool is_all_free(){
	return smalloc::get_manager().get_flat_address_space().begin()->get_state() == memory_allocation::chunk::busy ? false : true; 
}
bool is_empty(){
	return smalloc::get_manager().get_flat_address_space().empty();
}

void TestFunc(){

	    {
   		    	std::vector<int, smalloc::allocator<int>> v;
		        v = {1,2,3,4,5,5};
			const std::vector<int, smalloc::allocator<int>> expected = {1,2,3,5,5,78};
			v.erase(v.begin() + 3);
			v.push_back(78);
			assert(v == expected);
	    }
	    assert(is_all_free());
	    assert(is_empty());

	    {
		    std::vector<BigArray, smalloc::allocator<BigArray>> v;
		    std::vector<BigArray, smalloc::allocator<BigArray>> expected;	
		    expected.push_back(BigArray{'a'});
		    expected.push_back(BigArray{'b'});
		    expected.push_back(BigArray{'c'});
		    expected.push_back(BigArray{'d'});
		    expected.push_back(BigArray{'e'});
		    expected.push_back(BigArray{'L'});
		    v.push_back(BigArray{'a'});
		    v.push_back(BigArray{'b'});
		    v.push_back(BigArray{'c'});
		    v.push_back(BigArray{'d'});
		    v.push_back(BigArray{'e'});
		    v.push_back(BigArray{'f'});
		    v.erase(v.begin() + 5);
		    v.push_back(BigArray{'L'});
		    assert(v == expected);


	    }

	    assert(is_all_free());

	    assert(is_empty());

	    {
		    std::vector<int, smalloc::allocator<int>> v;
		    v = {1,2,3,4,5,5,};
		    const std::vector<int, smalloc::allocator<int>> expected = {1,2,3,5,5,78};
		    v.erase(v.begin() + 3);
		    v.push_back(78);
		    assert(v == expected);
		    std::list<BigArray, smalloc::allocator<BigArray>> v1;
		    std::list<BigArray, smalloc::allocator<BigArray>> expected1 = {BigArray{'a'}, BigArray{'b'}, BigArray{'c'},
											    BigArray{'d'}, BigArray{'f'}, BigArray{'L'}};
		    const std::list<BigArray, smalloc::allocator<BigArray>> expected2 = {BigArray{'a'}, BigArray{'b'}, BigArray{'c'},                                                                       
			                                                                     BigArray{'d'}, BigArray{'f'}}; 
		    v1.push_back(BigArray{'a'});
		    v1.push_back(BigArray{'b'});
		    v1.push_back(BigArray{'c'});
		    v1.push_back(BigArray{'d'});
		    v1.push_back(BigArray{'f'});
		    v1.push_back(BigArray{'g'});
		    v1.erase((std::next(v1.begin(),5)));
		    v1.push_back(BigArray{'L'});					
		    assert(v1 == expected1);						
		    v1.pop_back();
	            v1.swap(expected1);
	            assert(expected1 == expected2);
	    }
				        
	    assert(is_all_free());	
	    assert(is_empty());



}

int main(){


}



