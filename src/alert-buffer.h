#include "cyclic-buffer.h"

namespace HttpMonitorHelper
{
    struct AlertUnit
    {
        int totalTraffic;
    };

    class AlertBuffer:
        public CyclicBuffer<AlertUnit>
    {
        private:
            int m_interval;
            int m_totalTraffic = 0;
            int m_alertMin;
        public:
            AlertBuffer(int interval, int beat, int alertMin);

            // AlertBuffer::insert(data)
            //  Inserts data into buffer
            //  Returns true if the total traffic should trigger an alert
            bool insert(AlertUnit data);
    };
}