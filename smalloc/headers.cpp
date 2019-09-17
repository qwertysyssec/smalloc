#include "headers.hpp"



//workspace



void offset_and_assign( void*& address, std::size_t length){
    assert(address!=nullptr);
    assert((reinterpret_cast<std::size_t>(address)-1)<=(SIZE_MAX-length));
      address=reinterpret_cast<void*>(reinterpret_cast<std::size_t>(address)+length);
}


std::size_t rounded_number(std::size_t number){
    assert(number <= static_cast<std::size_t>(0u-max_aling ));
    return number + ((number%max_aling ) != 0 ?
                alignof(std::max_align_t)-(number%max_aling )  : 0);
}

//workspace
