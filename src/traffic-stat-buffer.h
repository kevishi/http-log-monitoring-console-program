#include "cyclic-buffer.h"

namespace HttpMonitorHelper
{
    struct TrafficUnit
    {
        int totalTraffic;
    };

    class TrafficBuffer:
        public CyclicBuffer<TrafficUnit>
    {
        private:
            int m_totalTraffic = 0;
        public:
            TrafficBuffer(int interval, int beat);

            // TrafficBuffer::insert(data)
            //  Inserts data into buffer, updates traffic data
            void insert(TrafficUnit data);
    };
}