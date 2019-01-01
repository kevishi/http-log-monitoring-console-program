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
            int m_beat;
            int m_totalTraffic = 0;
        public:
            TrafficBuffer(int interval, int beat);
            void insert(TrafficUnit data);
    };
}