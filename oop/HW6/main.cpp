#include <iostream>
#include "vector.h"
using namespace std;

int main() {
    Vector<int> vec;

    // Test push_back and size
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    cout << "Size after adding elements: " << vec.size() << endl;

    // Test access operator and at()
    cout << "Element at index 1: " << vec[1] << endl;
    try {
        cout << "Element at index 3: " << vec.at(3) << endl;
    } catch (const std::out_of_range& e) {
        cout << "Caught exception: " << e.what() << endl;
    }

    // Test clear and empty
    vec.clear();
    cout << "Is vector empty after clear? " << (vec.empty() ? "Yes" : "No") << endl;

    // Test copy constructor
    vec.push_back(40);
    vec.push_back(50);
    Vector<int> vecCopy = vec;
    cout << "Copy of vector, size: " << vecCopy.size() << endl;
    cout << "First element in copied vector: " << vecCopy[0] << endl;

    return 0;
}
