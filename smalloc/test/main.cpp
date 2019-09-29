#include <iostream>
#include <cassert>
#include "../include/manager.hpp"
#include <vector>
#include  <cassert>
#include <list>
#include <gtest/gtest.h>

bool is_all_free(){
	return smalloc::get_manager().get_flat_address_space().begin()->get_state() == memory_allocation::chunk::busy ? false : true; 
}
bool is_empty(){
	return smalloc::get_manager().get_flat_address_space().empty();
}


/*
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
            v.push_back(78);std::vector<int, smalloc::allocator<int>> v;
                v = {1,2,3,4,5,5};
            const std::vector<int, smalloc::allocator<int>> expected = {1,2,3,5,5,78};
            v.erase(v.begin() + 3);
            v.push_back(78);
            assert(v == expected);
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

*/

//alocator tests
TEST(allocator_test, bacic_check){
    {
        std::vector<int, smalloc::allocator<int>> v;
        v = {1,2,3,4,5,5};
        const std::vector<int, smalloc::allocator<int>> expected = {1,2,3,5,5,78};
        v.erase(v.begin() + 3);
        v.push_back(78);
        ASSERT_TRUE(v == expected);
    }

    ASSERT_TRUE(is_all_free());
    ASSERT_TRUE(is_empty());
}

TEST(allocator_test, test_with_big_objects){
    using BigArray = std::array<unsigned char, 300>;
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

        ASSERT_TRUE(v == expected);
    }
    ASSERT_TRUE(is_all_free());
    ASSERT_TRUE(is_empty());

}
TEST(allocator_test, interesting_test){
    using BigArray = std::array<unsigned char, 4700>;
    std::vector<BigArray, smalloc::allocator<BigArray>> v;
    v.push_back(BigArray{'v'});
    std::list<int> l;
    l.push_back(6);
    v.erase(v.begin());
    ASSERT_TRUE(smalloc::get_manager().get_flat_address_space().size() == 2);
    ASSERT_TRUE(!is_all_free());

}

TEST(chunk_test, creating_with_nullptr){
    EXPECT_ANY_THROW(memory_allocation::chunk(nullptr, 45, memory_allocation::chunk::busy));
}
TEST(chunk_test, creating_with_zero_size){
    EXPECT_ANY_THROW(memory_allocation::chunk(reinterpret_cast<void *>(12121), 0, static_cast<memory_allocation::chunk::busyness>(1)));
    EXPECT_THROW
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


