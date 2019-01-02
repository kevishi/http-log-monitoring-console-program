#include "cyclic-buffer.h"
#include "clf.h"
#include <string>
#include <unordered_map>

using namespace std;

namespace HttpMonitorHelper
{
    struct TrafficUnit
    {
        int totalTraffic = 0;
        int maxRequest = 0;
        int totalRequestSize = 0;
        int totalSuccess = 0;
        int totalFail = 0;

        unordered_map<string, int> sectionCounts;
        unordered_map<string, int> addrCounts;
        unordered_map<string, int> userCounts;
    };

    class TrafficBuffer:
        public CyclicBuffer<TrafficUnit>
    {
        private:
            int m_totalTraffic = 0;
            int m_maxRequest = 0;
            int m_totalRequestSize = 0;
            int m_totalSuccess = 0;
            int m_totalFail = 0;

            unordered_map<string, int> m_sectionCounts;
            unordered_map<string, int> m_addrCounts;
            unordered_map<string, int> m_userCounts;

        public:
            TrafficBuffer(int interval);

            // TrafficBuffer::insert(data)
            //  Inserts data into buffer, updates traffic data
            void insert(TrafficUnit data);

            // top3...
            //  Returns the top 3 of respective CLF fields of the last interval seconds
            vector<string> top3Sections();
            vector<string> top3addr();
            vector<string> top3user();

            // MaxRequestSize
            //  Returns the largest request size from the last interval seconds
            int maxRequestSize();

            // AverageRequestSize
            //  Returns the average request size from the last interval seconds
            int totalRequestSize();

            // NumSuccess
            //  Returns the number of successful statuses from the last interval seconds
            int numSuccess();

            // MaxRequestSize
            //  Returns the number of unsuccessful statuses from the last interval seconds
            int numFail();

            // Clear
            // Clears buffer
            void clear();
    };
}