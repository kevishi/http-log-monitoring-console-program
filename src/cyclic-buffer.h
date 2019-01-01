#include <queue>
#include <iostream>
#include <experimental/optional>

// CyclicBuffer<T>(size)
//  Buffer that holds size number of T (at maximum)
//  Any additional T added will remove first added element
template<typename T>
class CyclicBuffer
{
    private:
        int m_maxSize;
        int m_curSize = 0;
        std::queue<T> m_queue;

    public:
        CyclicBuffer(int size);

        // CyclicBuffer::insert(data)
        //  Inserts data into buffer, returns popped element if necessary
        std::experimental::optional<T> insert(T data);

        //TODO remove
        int get();
};

// template definition
template<typename T>
CyclicBuffer<T>::CyclicBuffer(int size): m_maxSize(size), m_queue()
{
}

template<typename T>
std::experimental::optional<T> CyclicBuffer<T>::insert(T data)
{
    std::experimental::optional<T> poppedElement();
    if(m_curSize == m_maxSize && m_curSize != 0)
    {
        poppedElement = m_queue.front();
        m_queue.pop();
        m_curSize--;
    }

    m_queue.push(data);
    m_curSize++;

    return poppedElement;
}

template<typename T>
int CyclicBuffer<T>::get()
{
    return 5;
}