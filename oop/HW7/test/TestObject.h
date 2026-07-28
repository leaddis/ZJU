// TestObject.h
#ifndef TEST_OBJECT_H
#define TEST_OBJECT_H

#include <iostream>

struct TestObject {
    int id;
    double value;

    TestObject() : id(0), value(0.0) {
        std::cout << "TestObject Default Constructor called.\n";
    }

    TestObject(int id_, double value_) : id(id_), value(value_) {
        std::cout << "TestObject Parameterized Constructor called. ID: " << id << ", Value: " << value << "\n";
    }

    ~TestObject() {
        std::cout << "TestObject Destructor called. ID: " << id << "\n";
    }
};

#endif // TEST_OBJECT_H
    