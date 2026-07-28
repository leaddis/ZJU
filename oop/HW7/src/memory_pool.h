// memory_pool.h
#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include "buffer.h"
#include <cstddef> // For size_t

template <typename T>
struct Block
{
    Block* next;
};

template <typename T>
class MemoryPool
{
private:
    Buffer<T>* m_headBuffer=new Buffer<T>;
    Buffer<T>* m_currentBuffer = NULL ;
    size_t m_bufferedBlocks= BUFFER_SIZE ;
    Block<T>* m_firstFreeBlock;
    Block<T>* m_bigMemory;

public:
    // Constructor
    MemoryPool()
        : m_headBuffer(new Buffer<T>()),
          m_currentBuffer(nullptr),
          m_bufferedBlocks(BUFFER_SIZE),
          m_firstFreeBlock(nullptr),
          m_bigMemory(nullptr)
    {
        m_headBuffer->setNext(NULL);
        m_firstFreeBlock = NULL;
    }

    // Delete move and copy constructors and assignment operators
    MemoryPool(MemoryPool &&memoryPool) = delete;
    MemoryPool(const MemoryPool &memoryPool) = delete;
    MemoryPool& operator=(MemoryPool &&memoryPool) = delete;
    MemoryPool& operator=(const MemoryPool &memoryPool) = delete;

    // Destructor
    ~MemoryPool()
    {
        while (m_headBuffer)
        {
            Buffer<T>* buffer = m_headBuffer;
            m_headBuffer = buffer->getNext();
            delete buffer;
        }
        // 释放大块内存
        while (m_bigMemory) {
            T* tempBlock = reinterpret_cast<T*>(m_bigMemory);
            m_bigMemory = m_bigMemory->next;
            ::operator delete(tempBlock);
        }
    }

    T* allocate(size_t n)
    {
        if(n == 1){
            if (m_firstFreeBlock)
            {
                Block<T>* block = m_firstFreeBlock;
                m_firstFreeBlock = block->next;
                return reinterpret_cast<T*>(block);
            }
            if (m_bufferedBlocks + n >= BUFFER_SIZE)
            {
                Buffer<T>* newBuffer = new Buffer<T>();
                newBuffer->setNext(m_headBuffer->getNext());
                m_headBuffer->setNext(newBuffer);
                m_currentBuffer = newBuffer;
                m_bufferedBlocks = 0;
            }   
            return m_currentBuffer->getBlock(m_bufferedBlocks++);
        }
        else if (n < 1){
            throw std::invalid_argument("n must be greater than 0");
        }
        else{
            //如果n大于BUFFER_SIZE，直接分配一个大内存块,不使用内存池
            if(n > BUFFER_SIZE){
                T* temp = (T*)::operator new(sizeof(T) * n);
                Block<T>* block = reinterpret_cast<Block<T>*>(temp);
                block->next = m_bigMemory;
                m_bigMemory = block;
                return temp;
            }
            if (m_bufferedBlocks + n >= BUFFER_SIZE) { // 若多分配n个超出容量
                Buffer<T>* newBuffer = new Buffer<T>;
                newBuffer->setNext(m_headBuffer->getNext());
                m_headBuffer->setNext(newBuffer);

                // 将剩余的块链接到 free list
                int index = m_bufferedBlocks;
                while (index < BUFFER_SIZE)
                {
                    Block<T>* tempBlock = reinterpret_cast<Block<T>*>(m_currentBuffer->getBlock(index));
                    tempBlock->next = m_firstFreeBlock;
                    m_firstFreeBlock = tempBlock;
                    index++;
                }

                // 切换到新的 Buffer
                m_currentBuffer = newBuffer;
                m_bufferedBlocks = 0;
            }

            m_bufferedBlocks += n;
            return m_currentBuffer->getBlock(m_bufferedBlocks - n);
        }
    }

    void delete_allocate(T* pointer)
    {
        //直接将block插入到空闲链表的头部
        Block<T> *block = reinterpret_cast<Block<T> *>(pointer);
        block->next = m_firstFreeBlock;
        m_firstFreeBlock = block;
        // ::operator delete(pointer);
    }
};

#endif // MEMORY_POOL_H
