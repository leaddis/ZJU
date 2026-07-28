#ifndef VECTOR_CPP
#define VECTOR_CPP

#include "vector.h"
template <class T>
Vector<T>::Vector() : m_pElements(nullptr), m_nSize(0), m_nCapacity(0) {}

template <class T>
Vector<T>::Vector(int size) : m_nSize(size), m_nCapacity(size) {
    m_pElements = new T[m_nCapacity];
}

template <class T>
Vector<T>::Vector(const Vector& r) : m_nSize(r.m_nSize), m_nCapacity(r.m_nCapacity) {
    m_pElements = new T[m_nCapacity];
    for (int i = 0; i < m_nSize; ++i) {
        m_pElements[i] = r.m_pElements[i];
    }
}

template <class T>
Vector<T>::~Vector() {
    delete[] m_pElements;
}

template <class T>
T& Vector<T>::operator[](int index) {
    return m_pElements[index];
}

template <class T>
T& Vector<T>::at(int index) {
    if (index < 0 || index >= m_nSize) {
        throw std::out_of_range("Index out of range");
    }
    return m_pElements[index];
}

template <class T>
int Vector<T>::size() const {
    return m_nSize;
}

template <class T>
void Vector<T>::push_back(const T& x) {
    if (m_nSize == m_nCapacity) {
        inflate();
    }
    m_pElements[m_nSize++] = x;
}

template <class T>
void Vector<T>::clear() {
    m_nSize = 0;
}

template <class T>
bool Vector<T>::empty() const {
    return m_nSize == 0;
}

template <class T>
void Vector<T>::inflate() {
    m_nCapacity = (m_nCapacity == 0 ? 1 : m_nCapacity * 2);
    T *newElements = new T[m_nCapacity];
    for (int i = 0; i < m_nSize; ++i) {
        newElements[i] = m_pElements[i];
    }
    delete[] m_pElements;
    m_pElements = newElements;
}

#endif // VECTOR_CPP