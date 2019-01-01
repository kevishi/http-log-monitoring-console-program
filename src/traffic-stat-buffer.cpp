#include "traffic-stat-buffer.h"

using namespace HttpMonitorHelper;

typedef std::experimental::optional<TrafficUnit> o_TrafficUnit;

TrafficBuffer::TrafficBuffer(int interval, int beat): CyclicBuffer(beat), m_interval(interval)
{
}

void TrafficBuffer::insert(TrafficUnit data)
{
   o_TrafficUnit poppedElem = CyclicBuffer::insert(data);
   if(poppedElem)
   {
       m_totalTraffic -= poppedElem.value().totalTraffic;
   }

   m_totalTraffic += data.totalTraffic;
}