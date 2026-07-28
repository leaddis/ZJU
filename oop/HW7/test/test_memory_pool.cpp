// test_memory_pool.cpp
#include "../src/memory_pool.h"
#include "TestObject.h"
#include <iostream>
#include <vector>
#include <stdexcept> // For std::invalid_argument

int main() {
    try {
        // Create a MemoryPool for TestObject
        MemoryPool<TestObject> pool;

        std::cout << "Allocating single TestObject instances...\n";

        // Allocate single objects
        TestObject* obj1 = pool.allocate(1);
        new (obj1) TestObject(1, 10.5); // Placement new to construct the object

        TestObject* obj2 = pool.allocate(1);
        new (obj2) TestObject(2, 20.5);

        // Use the objects
        std::cout << "obj1: ID = " << obj1->id << ", Value = " << obj1->value << "\n";
        std::cout << "obj2: ID = " << obj2->id << ", Value = " << obj2->value << "\n";

        // Deallocate obj1
        std::cout << "Deallocating obj1...\n";
        obj1->~TestObject(); // Manually call the destructor
        pool.delete_allocate(obj1);

        // Allocate another object, which should reuse obj1's memory
        TestObject* obj3 = pool.allocate(1);
        new (obj3) TestObject(3, 30.5);

        std::cout << "obj3: ID = " << obj3->id << ", Value = " << obj3->value << "\n";

        // Allocate multiple objects to test bulk allocation
        std::cout << "Allocating multiple TestObject instances...\n";
        const size_t numObjects = 10;
        std::vector<TestObject*> objects;
        for (size_t i = 4; i < 4 + numObjects; ++i) {
            TestObject* obj = pool.allocate(1);
            new (obj) TestObject(static_cast<int>(i), i * 10.0);
            objects.push_back(obj);
        }

        // Use the allocated objects
        for (auto obj : objects) {
            std::cout << "Allocated obj: ID = " << obj->id << ", Value = " << obj->value << "\n";
        }

        // Deallocate all objects
        std::cout << "Deallocating all objects...\n";
        obj2->~TestObject();
        pool.delete_allocate(obj2);

        obj3->~TestObject();
        pool.delete_allocate(obj3);

        for (auto obj : objects) {
            obj->~TestObject();
            pool.delete_allocate(obj);
        }

        std::cout << "All objects deallocated. Exiting program.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
