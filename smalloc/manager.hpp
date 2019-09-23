#ifndef UUID_CE87A25E_9A55_4AF7_AEA0_082A3AC1F90E
#define UUID_CE87A25E_9A55_4AF7_AEA0_082A3AC1F90E

#include "smalloc.hpp"
#include <limits>
#include <memory>



static memory_allocation::memory_manager manager;

namespace smalloc {

// interfaces for manual memory handling

    [[nodiscard]] void* allocate(std::size_t n){
           return manager.new_(n);
    }

    void deallocate(void* p){
        manager.delete_(p);
    }

    template< typename U, typename... Args >
    void construct( U* p, Args&&... args ){
        ::new(reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template< typename U >
    void destroy( U* p ){
        p->~U();
    }

//allocator

template<typename T>
class allocator{
  static_assert(std::is_destructible<T>::value, "your object is not destructible");
  public:
      using value_type = T;
      using pointer = T*;
      using const_pointer = const T*;
      using void_pointer = void*;
      using const_void_pointer = const void*;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
      using reference = T&;
      using const_reference = const T&;
      using propagate_on_container_move_assigment = std::true_type;
      using is_always_equal = std::true_type;


    template<typename U>
    struct rebind
    {
        using other = allocator<U>;
    };

    constexpr allocator() noexcept {}
    constexpr allocator(const allocator& other) noexcept {}

    template<typename U>
    constexpr allocator(const allocator<U>& other) noexcept {}


    [[nodiscard]] T* allocate(std::size_t n){
        return reinterpret_cast<T*>(manager.new_(n*sizeof(T)));
    }

    void deallocate(pointer p, std::size_t n){
        manager.delete_(reinterpret_cast<void*>(p));
    }

    pointer address( reference x ) const noexcept{
      return std::addressof(x);
    }

    const_pointer address( const_reference x ) const noexcept{
      return std::addressof(x);
    }

    size_type max_size() const noexcept{
      return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    template< typename U, typename... Args >
    void construct( U* p, Args&&... args ){
      ::new(reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template< typename U >
    void destroy( U* p ){
        p->~U();
    }

};

template< typename T1, typename T2 >
bool operator==( const allocator<T1>& lhs, const allocator<T2>& rhs ) noexcept {
    return true;
}
template< typename T1, typename T2 >
bool operator!=( const allocator<T1>& lhs, const allocator<T2>& rhs ) noexcept {
    return false;
}


}


#endif
