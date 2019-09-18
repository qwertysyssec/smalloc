#include "manager.hpp"








int main(){




    memory_allocation::memory_manager a;


    //std::array <unsigned char, SSIZE_MAX> aaa; - PIZDEC

    //a.t_mmap(1);
    auto t = a.new_(5);
    auto t3 = a.new_(2.3);
    auto t45 = a.new_(std::string("asdasdasdasd"));
    auto t4 = a.new_(std::string("asdasdasd"));






    for(const auto& h: a.flat_address_space.get<1>()){
     if(h.get_state() == memory_allocation::chunk::busy) std::cout << h.get_size() << " ";
    }
    std::cout << std::endl;

    for(const auto& h: a.flat_address_space.get<1>()){
     std::cout << h.get_size() << " ";
    }
    std::cout << std::endl;
    for(const auto& h: a.flat_address_space.get<1>()){
     std::cout << h.get_address() << " ";
    }

    a.delete_(t);
    //a.delete_(t);
    //a.delete_(t3);
    std::cout << std::endl;

    for(const auto& h: a.flat_address_space.get<1>()){
     std::cout << h.get_size() << " ";
    }
    std::cout << std::endl;
    for(const auto& h: a.flat_address_space.get<1>()){
     std::cout << h.get_address() << " ";
    }
    std::cout << std::endl;
    for(const auto& h: a.flat_address_space.get<1>()){
     if(h.get_state() == memory_allocation::chunk::busy) std::cout << h.get_size() << " ";
    }
    std::cout << std::endl;












    return 0;
}
