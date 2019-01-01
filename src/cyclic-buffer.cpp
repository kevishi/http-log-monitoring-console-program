#include "cyclic-buffer.h"

using namespace HttpMonitorHelper;

CyclicBuffer::CyclicBuffer(int size): m_size(size), m_queue()
{
}

opt<T> CyclicBuffer::insert(T data)
{

}
//TODO remove
int CyclicBuffer::get()
{
    return 5;
}