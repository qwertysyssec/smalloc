#ifndef UUID_9ABFA3E9_4594_4240_86B6_A4CCA900AD68
#define UUID_9ABFA3E9_4594_4240_86B6_A4CCA900AD68


#include "headers.hpp"
#include <cstring>                              /* memcpy */
#include <iostream>                             /* xorshift */
#include <cstddef>                              /* size_t */
#include <cassert>

// "//??" -- comments by  Pasha


namespace memory_allocation {



/* RandomNumberEngine */
class xorshift{

    std::uint64_t seed_;

public:

    using result_type = std::uint64_t;
    xorshift(std::uint64_t seed):seed_(seed){}
    xorshift():seed_(1231231212){}
    xorshift(const xorshift& tmp):seed_(tmp.seed_){}

    void seed(result_type seed){
        seed_ = seed;
    }
    void seed(){
        seed_ =  1231231212;
    }

    void discard(std::size_t num){
        for(uint i = 0; i < num; ++i){
            this->operator ()();
        }
    }
    bool operator == (const xorshift& rhs){
        return seed_ == rhs.seed_;
    }
    bool operator != (const xorshift& rhs){
        return seed_ != rhs.seed_;
    }

    friend std::istream& operator >> (std::istream& in, xorshift& tmp){
        in >> tmp.seed_;
        return in;
    }

    friend std::ostream& operator << (std::ostream& out, xorshift& tmp ){
       out << tmp.seed_;
       return out;
    }
    result_type operator ()(){
    seed_ ^= seed_<< 13;
    seed_ ^= seed_ << 7;
    seed_ ^= seed_ << 17;
    return seed_;
    }
};





    /* FUNCTION OBJECT */
    template<typename RNE = xorshift>
    class    random_page_filler{

        //?? if RNE if not movable or copyable we cant construct this
        static_assert(std::is_copy_assignable<RNE>::value || std::is_move_assignable<RNE>::value, "RNE isnt copieble or moveble");

        /*base generator*/
        /*concept RandomNumberEngine*/
        RNE gen;

        /*size of base engine result type*/
        std::size_t result_type_size=sizeof(typename RNE::result_type);

    public:

    /* constructors */

        /* default */
        random_page_filler():gen(RNE()){}

        /* copy */
        random_page_filler(const volatile random_page_filler& tmp) = delete; //?? only for now because random copy can make ub

        /* move */
        random_page_filler(random_page_filler&& tmp) = delete; //?? same


        random_page_filler(const RNE& gen_):gen(gen_){}
        random_page_filler(RNE&& gen_):gen(std::forward<RNE&&>(gen_)){}



        //?? seeds our generator
        void seed(){
            gen.seed();
        }
        void seed(typename RNE::result_type value){
            gen.seed(value);
        }


        //span filling
        void operator()(void* address, std::size_t length /*in bytes*/){

        //asserts
            assert(address!=nullptr);
            assert((reinterpret_cast<std::size_t>(address)-1)<=(SIZE_MAX-length));

            if(length==0) return;

            //initial state
            std::size_t filled=0;

            //filling
            while(filled!=length){
             //generate random number
                auto rand_num=gen();
             //choose size of fragment to memory-to-memory copy
                std::size_t fragment=std::min(result_type_size, length-filled);
             //filling the span
                std::memcpy(address, reinterpret_cast<void*>(&rand_num), fragment);
             //offset address
                offset_and_assign(address, fragment);
             //upload size of already filled span
                filled+=fragment;
            }

        }

    };


    //?? checks are generators same or not
    template<typename T, typename U>
    bool operator == (const random_page_filler<U>& lhs, const random_page_filler<T>& rhs){
        return std::is_same<U, T>::value;
    }
    template<typename T, typename U>
    bool operator != (const random_page_filler<U>& lhs, const random_page_filler<T>& rhs){
        return !std::is_same<U, T>::value;
    }



}

#endif
