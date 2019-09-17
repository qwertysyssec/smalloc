#ifndef UUID_1C682FDD_C224_4A65_9095_24FD90DD9D9F
#define UUID_1C682FDD_C224_4A65_9095_24FD90DD9D9F

#include "headers.hpp"
#include "smalloc_random.hpp"                   //random_page_filler
#include <set>                                  //memory_manager::new_
#include <utility>                              //memory_manager::new_
#include <boost/multi_index_container.hpp>      //multiindex
#include <boost/multi_index/ordered_index.hpp>  //multinidex
#include <boost/multi_index/mem_fun.hpp>        //multiindex
#include <cstddef>                              //std::size_t
#include <sys/mman.h>                           //mmap, posix_madvice
#include <cassert>                              //assert
#include <type_traits>

namespace memory_allocation{

  namespace {

    /* chunk - struct, what represent liner chunk of bytes in flat address space */

class chunk{
public:

    enum busyness{                      /*state of memory*/
        free,                 /*reserved by mmap and free*/
        busy                  /*reserved by mmap and busy*/
            };

    //safe chunk constraction
    chunk(void*            address_,
          std::size_t         size_,
          busyness           state_)
    : address(address_), size(size_), state(state_)
    {

        //asserts

        //address is multiple of max_align
        assert((max_aling-1)==((~reinterpret_cast<std::size_t>(address))&(max_aling-1)));
        //address is not nullptr
        assert(nullptr!=address);

        //size is multiple of max_align
        assert((max_aling-1)==((~size)&(max_aling-1)));
        //size is not null
        assert(0!=size);

        //chunk does not cross the border between SIZE_MAX and NULL
        assert((reinterpret_cast<std::size_t>(address)-1)<=(SIZE_MAX-size));

    }

    //no default contructor
    chunk() = delete;

    //getters

    //address
    void* get_address() const {
        return address;
    }
    //size
    std::size_t get_size() const{
        return size;
    }
    //state
    auto get_state() const {
        return state;
    }



    //setters

    //safe change of address
    void change(void* address_){
        address=address_;
        //asserts
        assert((max_aling-1)==((~reinterpret_cast<std::size_t>(address))&(max_aling-1)));
        assert(nullptr!=address);
        assert((reinterpret_cast<std::size_t>(address)-1)<=(SIZE_MAX-size));
    }

    //safe change of size
    void change(std::size_t size_){
        size=size_;
        //asserts
        assert((max_aling-1)==((~size)&(max_aling-1)));
        assert(0!=size);
        assert((reinterpret_cast<std::size_t>(address)-1)<=(SIZE_MAX-size));
    }

    //safe change of state
    void change(busyness state_){
        state=state_;
    }

    bool operator < (const chunk& rhs) const{
        if(state > rhs.state) return true;
        else if(state < rhs.state)  return false;
        else if(state == rhs.state){
            if(size < rhs.size) return true;
            else return false;
        }
    }


private:
    void*   address;          /*the begining of linear chunk*/
    std::size_t size;         /*size in bytes, max size of chunk == ~(max_align-1)*/
    busyness state;           /*state of memory*/
};

  }

    /*memory_manager class provide overall shell around memory allocation*/

    class memory_manager{


    public:


        //new function

        void* new_(std::size_t num_of_bytes){

             assert(num_of_bytes!=0);
            const std::size_t rounded_num_of_bytes = rounded_number(num_of_bytes);

            //count current wideness of choise in container
                std::size_t current_wideness=0;
                {

                auto iter=flat_address_space.lower_bound(chunk{reinterpret_cast<void*>(max_aling), rounded_num_of_bytes, chunk::free});

                auto end=flat_address_space.end();



                while(iter!=end
                      &&
                      current_wideness<wideness_of_choise
                      ){

                    current_wideness+=iter->get_size()/rounded_num_of_bytes;
                    ++iter;
                }
                }

                current_wideness=std::min(current_wideness, wideness_of_choise);


                //random number of place to place object
                 auto vector = dist(gen);

                 //if random number of place is not more than current wideness
                if(vector<=current_wideness){

                    //iterator to consisting place for new object chunk
                    std::size_t i=0;
                    auto iter=flat_address_space.lower_bound(chunk(reinterpret_cast<void*>(max_aling),rounded_num_of_bytes,chunk::free));
                    while(i<vector){ i+=iter->get_size()/rounded_num_of_bytes; if(i<vector) ++iter;}

                    //offset of object in chunk
                    auto offset=(vector-(i-iter->get_size()/rounded_num_of_bytes)-1)*rounded_num_of_bytes;

                    //create temporary chunk same as consisting place for object chunk
                     chunk t_chunk (iter->get_address(), iter->get_size(), iter->get_state());
                     //erase consisting place for object chunk
                     flat_address_space.erase(iter);
                     //insert busy chunk
                     auto b_chunk_iter=flat_address_space.insert(chunk(reinterpret_cast<void*>(reinterpret_cast<std::size_t>(t_chunk.get_address())+offset),
                                                                       rounded_num_of_bytes,
                                                                       chunk::busy)
                                                                 ).first;


                     //if left free space exist insert free left chunk
                     if(offset)
                         flat_address_space.insert(chunk(t_chunk.get_address(),
                                                         offset,
                                                         chunk::free)
                                                   );

                     //if right free space exist insert free right chunk
                     if(offset+rounded_num_of_bytes<t_chunk.get_size())
                         flat_address_space.insert(chunk(reinterpret_cast<void*>(reinterpret_cast<std::size_t>(b_chunk_iter->get_address())+rounded_num_of_bytes),
                                                         t_chunk.get_size()-(offset+rounded_num_of_bytes),
                                                         chunk::free)
                                                   );
                 return b_chunk_iter->get_address();
                }




                //if(vector>сurrent_wideness)
                else
                    {
                         vector-=current_wideness;

                         //create containet
                         std::set<void*> other_address_index;

                         //rounded_num_of_bytes, converted in rounded up number of paged
                         std::size_t num_of_pages_in_T =rounded_num_of_bytes/page_size+(rounded_num_of_bytes%page_size)?1:0;

                         //size of chunks
                         std::size_t span_size=num_of_pages_in_T*page_size;

                         //make appropriate wideness of choise
                         for(std::size_t i=0; i<vector; [&]{
                                                                             if(rounded_num_of_bytes<=page_size/2) i+=page_size/rounded_num_of_bytes;
                                                                             else ++i;
                                                                             }()
                             )
                         other_address_index.insert(w_mmap(num_of_pages_in_T));

                         //iterator to object's place node
                         std::size_t i=0;
                         auto iter=other_address_index.begin();
                         while(i<vector){ i+=span_size/rounded_num_of_bytes; if(i<vector) ++iter;}

                         //safe address of the begining of the chunk
                         std::size_t address=reinterpret_cast<std::size_t>(*iter);

                         //unmap unused memory
                         for(auto _ : other_address_index)
                             if(reinterpret_cast<std::size_t>(_)!=address) munmap( _, num_of_pages_in_T );

                         //offset
                         auto offset=((vector)-(i-span_size/rounded_num_of_bytes)-1)*rounded_num_of_bytes;

                         //address index
                         auto& address_index=flat_address_space.get<1>();

                         //insert busy chunk
                         const auto b_chunk_iter=address_index.insert(chunk(reinterpret_cast<void*>(address+offset),
                                                                            rounded_num_of_bytes,
                                                                            chunk::busy)).first;

                         //workspace

                         /* а) освобождаем левый чанк
                          * 1) округлить вниз верхнюю грань
                          * 2) сравнить
                          * 3) если интервал больше чем равен размеру страницы, освободить его
                          * 4) обновить данный (address и т д )
                          * б) освобождаем правый чанк
                          * 1) округлить вверх нижнюю границу
                          * 2)  сравнить
                          * 3)если интервал больше чем или равен размеру страницы, освободить его
                          * 4)обновить данные(address, границы и т д)
                          */

                         //unmap left free span
                         if(offset){
                         auto right_broad=reinterpret_cast<std::size_t>(b_chunk_iter->get_address())
                                          -
                                         (offset%page_size);
                         auto left_broad=address;
                             if(left_broad<right_broad){
                                 munmap(reinterpret_cast<void*>(left_broad), right_broad-left_broad);
                                 span_size-=right_broad-left_broad;
                                 address=right_broad;
                                 offset-=right_broad-left_broad;
                             }
                         }

                         //unmap right free span
                         if(span_size-(offset+rounded_num_of_bytes)){
                         auto right_broad=address+span_size;
                         auto left_broad=address+offset+rounded_num_of_bytes+(page_size-(offset+rounded_num_of_bytes)%page_size)%page_size;
                             if(left_broad<right_broad){
                                 munmap(reinterpret_cast<void*>(left_broad), right_broad-left_broad);
                                 span_size-=right_broad-left_broad;
                             }
                         }

                         //workspace

                         //handling free chunks with defragmentation


                         //left span defragmentation
                         if(offset){
                             chunk middle(reinterpret_cast<void*>(address),
                                          offset,
                                          chunk::free
                                          );
                             //if left chunk exist, free and adjacent to middle chunk
                             if(b_chunk_iter!=address_index.begin()){
                                 auto left_iter=b_chunk_iter;
                                    --left_iter;
                                 if(
                                    left_iter->get_state()==chunk::free
                                    &&
                                    adjacency(*left_iter, middle)
                                   )
                                     //change address
                                     middle.change(left_iter->get_address());
                                     //change size
                                     middle.change(middle.get_size()+left_iter->get_size());
                                     //erase absorbed chunk
                                     address_index.erase(left_iter);

                             }
                             //insert absorbent chunk
                             address_index.insert(middle);
                         }


                             //right span defragmentation
                         if(span_size-(offset+rounded_num_of_bytes)){
                             chunk middle(reinterpret_cast<void*>(address+offset+rounded_num_of_bytes),
                                          span_size-(offset+rounded_num_of_bytes),
                                          chunk::free
                                          );
                             //if right chunk exist, free and adjacent to middle chunk
                             if(b_chunk_iter!=--address_index.end()){
                                 auto right_iter=b_chunk_iter;
                                    ++right_iter;
                                 if(
                                     right_iter->get_state()==chunk::free
                                     &&
                                     adjacency(middle, *right_iter)
                                   ){
                                     //change size
                                     middle.change(middle.get_size()+right_iter->get_size());
                                     //erase absorbed chunk
                                     address_index.erase(right_iter);
                                 }
                             }

                             //insert absorbent chunk
                             address_index.insert(middle);
                         }



                         return b_chunk_iter->get_address();
                }
         }


        //delete function

        void delete_(void* ptr){

            //index by address
            auto& address_index=flat_address_space.get<1>();

            //chunk to free
            const auto middle=address_index.find(ptr);

            //error handling: invalid pointer
            if(middle == address_index.end())    throw std::invalid_argument("delete: invalid pointer.");

            //error handling: double free
            if(middle->get_state()==chunk::free) throw std::invalid_argument("delete: double free.");


            //delete binary information
            page_initializer(middle->get_address(), middle->get_size());

            //create composite chunk
            chunk composite_chunk(middle->get_address(), middle->get_size(), chunk::free);

        //defragmentation

            //defragmentation with chunk on the left
            //if left chunk exist, is free and adjacent with composite_chunk
            if(middle!=address_index.begin()){

                auto left= middle;
                   --left;

               if((left->get_state()==chunk::free) && adjacency(*left, *middle))
                {
            //compose left chunk with composite_chunk
                    //change address
                    composite_chunk.change(left->get_address());
                    //change size
                    composite_chunk.change(composite_chunk.get_size()+left->get_size());
                    //erase left chunk
                    address_index.erase(left);
                }
        }
            //defragmentation with chunk on the right
            //if right chunk exist, is free and adjacent with composite_chunk
            if(middle!=(--address_index.end())){

                auto right=middle;
                   ++right;

               if((right->get_state()==chunk::free) && adjacency(*middle, *right))
                {
            //compose right chunk with composite_chunk
                    //no change address

                    //change size
                    composite_chunk.change(composite_chunk.get_size()+right->get_size());
                    //erase right chunk
                    address_index.erase(right);
                }

        }

            //erase middle chunk
            address_index.erase(middle);

        //releasing free pages
            const std::size_t c_address=reinterpret_cast<std::size_t>(composite_chunk.get_address());
            const std::size_t c_size=composite_chunk.get_size();

            if(c_size>=page_size){


                //left border of releasing span [
            const std::size_t left_border=c_address+((page_size-(c_address%page_size))%page_size);


            //right border of releasing span )
            const std::size_t right_border=(c_address+c_size)-((c_address+c_size)%page_size);



            //checking conditions
            if(right_border>left_border){


                munmap(reinterpret_cast<void*>(left_border), (right_border-left_border));

                //insert left free not munmapped part if it exist
                if(c_address<left_border) 	  flat_address_space.insert(chunk(reinterpret_cast<void*>(c_address), (left_border-c_address), chunk::free));

                //insert right free not munmapped part if it exist
                if(right_border<c_address+c_size) flat_address_space.insert(chunk(reinterpret_cast<void*>(right_border),(c_address+c_size-right_border),chunk::free));
            }
            else flat_address_space.insert(composite_chunk);



            }else flat_address_space.insert(composite_chunk);


        }


        ~memory_manager(){

        {
            //delete binary information
            auto iter=flat_address_space.lower_bound(
                                                        chunk(reinterpret_cast<void*>(max_aling),
                                                              max_aling,
                                                              chunk::busy
                                                        )
                                                    );
            while(iter!=flat_address_space.end()){
                page_initializer(iter->get_address(), iter->get_size());
                ++iter;
            }
        }

            //munmap
            auto& address_index=flat_address_space.get<1>();

            auto begin=address_index.begin();
            auto end=address_index.end();

            while(begin!=end){
                void* address=begin->get_address();
                std::size_t length=begin->get_size();

                auto next=begin;
                ++next;

              while(next!=end&&adjacency(*begin, *next)){

                  length+=next->get_size();

                  ++begin;
                  ++next;
              }
              munmap(address, length);
              ++begin;
            }

        }


protected:

                   /* boost_multi_index type alias provided just for programmers comfortability*/

               using boost_multi_index = boost::multi_index_container<
               chunk, boost::multi_index::indexed_by<
                         boost::multi_index::ordered_non_unique <boost::multi_index::identity<chunk>>,
                         boost::multi_index::ordered_unique    <boost::multi_index::const_mem_fun<chunk, void*, &chunk::get_address>>

               >>;

                   /* flat_address_space is a data structure, representing flat adress space.
                    * it stores chunks, providing access by three interfaces:
                    *                      1) ordered by chunks (by operator < : busy < free, free ordered by size ->)
                    *                      2) ordered by address
                    */

                   boost_multi_index flat_address_space;


                   /* initialize a linear chunk of bytes to random state */
                   random_page_filler<xorshift> page_initializer;

                   /* w_mmap is a a wrap around mmap syscall.
                      return value - void*.
                      parameter - size_t num_of_pages - amount of pages
                      requered in the linear chunk.
                   */

                   void* w_mmap(std::size_t num_of_pages){

                       /* address==nullptr   (shows to kernel that it does not matter
                        *                 where to locate this mapping, provide better
                        *                                              portability)
                        * length        - length of required linear chunk in bytes.
                        * PROT_READ     - allow the mapped space to be read from
                        * PROT_WRITE    - allow the mapped space to be written to
                        * MAP_PRIVATE   - makes mapping invisible to other processes
                        * MAP_ANONYMOUS - the  mapping  is  not  backed  by any file;
                        *                 its contents are initialized to zero.
                        *                 the fd argument is ignored (-1 for portability).
                        *                 the offset  argument  should be zero.
                        *
                        * fd==-1    (because of MAP_ANONYMOUS and for portability)
                        * offset==0 (because of MAP_ANONYMOUS)
                        */

                       std::size_t length=num_of_pages*page_size; /*in bytes*/

                       void* p=mmap(nullptr,                                   /*address*/
                                    length,                                    /*length of required linear chunk in bytes*/
                                    PROT_READ|PROT_WRITE,                      /*protection*/
                                    MAP_PRIVATE|MAP_ANONYMOUS,                 /*flags*/
                                   -1,                                         /*file descriptor*/
                                    0 );                                       /*offset*/
                       if(p==MAP_FAILED);//error handling

                       mlock(p, length);                   /*lock in RAM, no swap (for security)*/

                       //initializing space to random state
                       page_initializer(p, length);

                       return p;
                   }

        /* page_size - stores the size of page in bytes (RUNTIME)*/

        const std::size_t page_size=[]{int ret=sysconf(_SC_PAGESIZE);
                                 if((-1)==ret);//error handling
                                 return static_cast<std::size_t>(ret);
                                            }();

        const std::size_t wideness_of_choise = 10;

           std::mt19937 gen;
           std::uniform_int_distribution<std::size_t> dist=std::uniform_int_distribution<std::size_t> (1, wideness_of_choise);

        bool adjacency(const chunk& left, const chunk& right){

        return          reinterpret_cast<std::size_t>(right.get_address())
                            -
                        reinterpret_cast<std::size_t>(left.get_address())
                        ==
                        left.get_size();
        }

#ifndef NDEBUG
    public:

    //cant return reference or pointer to w_mmap
    //cant return reference or pointer to adjacency

    const auto& get_flat_address_space() const {
        return flat_address_space;
    }
    const auto& get_page_initializer() const {
        return page_initializer;
    }
    const auto& get_page_size() const {
        return page_size;
    }
    const auto& get_wideness_of_choice() const {
        return wideness_of_choise;
    }
    const auto& get_dist() const {
        return dist;
    }
    const auto&  get_gen() const {
        return gen;
    }


#endif


    };



}
#endif
