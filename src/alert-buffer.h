#include "cyclic-buffer.h"

namespace HttpMonitorHelper
{
    struct AlertUnit
    {
        int totalTraffic;
    };
    
    // AlertBuffer(interval, beat, alertMin)
    //  Buffer that holds size number of T (at maximum)
    //  Any additional T added will remove first added element
    //  0 size is undefined behavior
    //
    //  alertMin:
    //  if the amount of traffic is greater than alertMin it will send an alert on insert
    //  
    //  size: size of buffer
    class AlertBuffer:
        public CyclicBuffer<AlertUnit>
    {
        private:
            int m_totalTraffic = 0;
            int m_alertMin;
        public:
            AlertBuffer(int size, int alertMin);

            // AlertBuffer::insert(data)
            //  Inserts data into buffer
            //  Returns true if the total traffic should trigger an alert
            bool insert(AlertUnit data);

            int getTotalTraffic() { return m_totalTraffic; }
    };
}