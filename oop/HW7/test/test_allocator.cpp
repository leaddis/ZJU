// test_memory_pool_allocator.cpp
#include "../src/allocator.h"
#include "TestObject.h"

int main() {
    try {
        // Create a vector of TestObject using MemoryPoolAllocator
        std::vector<TestObject, MyAllocate<TestObject>> vec;

        std::cout << "Adding TestObjects to vector...\n";

        // Add elements to the vector
        vec.emplace_back(1, 10.5);
        vec.emplace_back(2, 20.5);
        vec.emplace_back(3, 30.5);

        std::cout << "\nCurrent vector contents:\n";
        for (const auto& obj : vec) {
            std::cout << "TestObject ID: " << obj.id << ", Value: " << obj.value << "\n";
        }

        std::cout << "\nRemoving the second element...\n";
        vec.erase(vec.begin() + 1); // Remove the element with ID 2

        std::cout << "\nVector contents after removal:\n";
        for (const auto& obj : vec) {
            std::cout << "TestObject ID: " << obj.id << ", Value: " << obj.value << "\n";
        }

        std::cout << "\nClearing the vector...\n";
        vec.clear();

        std::cout << "Vector cleared. Exiting program.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
