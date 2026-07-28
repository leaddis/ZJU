#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <cstdio>      // For printf
#include <cstddef>     // For std::size_t and std::ptrdiff_t
#include <new>         // For placement new and std::bad_alloc
#include <utility>     // For std::forward
#include <vector>      // STL vector container
#include <memory>      // For allocator interface
#include <type_traits>  // For std::is_same
#include <climits>      // For UINT_MAX

using namespace std;

// It's generally not recommended to use 'using namespace std;' in headers.
// Instead, use fully qualified names or limit 'using' directives to implementation files.

// #define BUFFER_SIZE 1024
constexpr size_t BUFFER_SIZE = 1024; // Prefer constexpr over #define

template <typename T>
class Buffer
{
private:
    // blockSize must at least hold the size of T
    static const size_t blockSize = sizeof(T);
    // Allocate space for BUFFER_SIZE blocks
    unsigned char* data = new unsigned char[blockSize * BUFFER_SIZE];
    Buffer* next;

public:
    Buffer() = default;

    // Destructor to clean up allocated memory
    // ~Buffer()
    // {
    //     delete[] data;
    // }

    // // Disable copy constructor and copy assignment to prevent shallow copies
    // Buffer(const Buffer&) = delete;
    // Buffer& operator=(const Buffer&) = delete;

    // // Enable move constructor and move assignment
    // Buffer(Buffer&& other) noexcept : data(other.data), next(other.next)
    // {
    //     other.data = nullptr;
    //     other.next = nullptr;
    // }

    // Buffer& operator=(Buffer&& other) noexcept
    // {
    //     if (this != &other)
    //     {
    //         delete[] data;
    //         data = other.data;
    //         next = other.next;
    //         other.data = nullptr;
    //         other.next = nullptr;
    //     }
    //     return *this;
    // }

    // Buffer linked list
    Buffer* getNext() const;
    void setNext(Buffer* p);

    // Get pointer to the block at the specified index
    T* getBlock(size_t index);
};

template <typename T>
Buffer<T>* Buffer<T>::getNext() const
{
    return next;
}

template <typename T>
void Buffer<T>::setNext(Buffer* p)
{
    next = p;
}

template <typename T>
T* Buffer<T>::getBlock(size_t index)
{
    return reinterpret_cast<T*>(data + index * blockSize);
}

#endif // BUFFER_H
