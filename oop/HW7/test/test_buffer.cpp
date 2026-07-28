// test_buffer.cpp
#include "../src/buffer.h" // Adjust the path as necessary
#include <new>             // For placement new
#include <iostream>

// Function to test the Buffer class
int main()
{
    // Create a Buffer for integers
    Buffer<int> intBuffer;

    // Initialize some blocks in the buffer using placement new
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
    {
        int* p = intBuffer.getBlock(i);
        if (p)
        {
            // Construct an integer in place
            new (p) int(static_cast<int>(i * 10)); // Example: storing multiples of 10
        }
    }

    // Retrieve and print some values from the buffer
    std::cout << "Retrieving values from Buffer<int>:" << std::endl;
    for (size_t i = 0; i < 10; ++i) // Print first 10 values
    {
        int* p = intBuffer.getBlock(i);
        if (p)
        {
            std::cout << "Block " << i << ": " << *p << std::endl;
        }
    }

    // No need to delete the buffer explicitly as the destructor handles it

    return 0;
}
