// allocator.h
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "memory_pool.h"
#include <memory> // For std::allocator
#include <cstddef>
#include <type_traits>


template<typename T>
class MyAllocate {
private:
    // Pointers to handle copying and rebinding
    MyAllocate* copyMyAllocate = NULL;
    std::allocator<T>* rebindMyAllocate = NULL;
    MemoryPool<T> memoryPool; // Define memory pool

public:
    // Type definitions
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template<class U>
    struct rebind {
        typedef MyAllocate<U> other;
    };

    // Constructors
    MyAllocate() = default;

    MyAllocate(MyAllocate& allocator) : copyMyAllocate(&allocator) {}

    template <class U>
    MyAllocate(const MyAllocate<U>& other)
    {
        if (!std::is_same<T, U>::value)
            rebindMyAllocate = new std::allocator<T>();
    }

    // Destructor
    ~MyAllocate(){
        delete copyMyAllocate;
        delete rebindMyAllocate;
    }

    // Allocation
    pointer allocate(size_type n, const void* hint = 0) {
    if (copyMyAllocate)
        return copyMyAllocate->allocate(n, hint);
    if (rebindMyAllocate)
        return rebindMyAllocate->allocate(n, hint);
    return memoryPool.allocate(n); // 确保 memoryPool.allocate 返回有效的指针
    }

    // Deallocation
    void deallocate(pointer p, size_type n)
    {
        if (copyMyAllocate) {
            copyMyAllocate->deallocate(p, n);
            return;
        }
        if (rebindMyAllocate) {
            rebindMyAllocate->deallocate(p, n);
            return;
        }
        memoryPool.delete_allocate(p);
    }

    // Construct and Destroy
    void construct(pointer ptr, const value_type& value) {
        new (ptr) T(value); // Placement new
    }

    void destroy(pointer ptr) {
        ptr->~T();
    }

    // Address
    pointer address(reference x) {
        return (pointer)&x;
    }

    const_pointer address(reference x) const {
        return (const_pointer)&x;
    }

    // Max size
    size_type max_size() const {
        return size_type(UINT32_MAX / sizeof(T));
    }

    // New Element
    pointer newElement(const value_type& value)
    {
        pointer result = allocate();
        construct(result, value);
        //return result;
    }

    // Delete Element
    void deleteElement(pointer p)
    {
        if (p != nullptr)
        {
            p->~value_type();
            deallocate(p);
        }
    }
};

// Allocators are stateless; hence, instances of allocators can be compared
template <typename T, typename U>
bool operator==(const MyAllocate<T>&, const MyAllocate<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const MyAllocate<T>&, const MyAllocate<U>&) { return false; }

#endif // ALLOCATOR_H
