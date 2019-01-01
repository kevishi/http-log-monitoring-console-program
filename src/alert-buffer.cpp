#include "alert-buffer.h"

using namespace HttpMonitorHelper;

typedef std::experimental::optional<AlertUnit> o_AlertUnit;

AlertBuffer::AlertBuffer(int interval, int beat, int alertMin):
    CyclicBuffer(beat), m_interval(interval), m_alertMin(alertMin)
{
}

bool AlertBuffer::insert(AlertUnit data)
{
   o_AlertUnit poppedElem = CyclicBuffer::insert(data);
   if(poppedElem)
   {
       m_totalTraffic -= poppedElem.value().totalTraffic;
   }

   m_totalTraffic += data.totalTraffic;

   return m_totalTraffic >= m_alertMin;
}