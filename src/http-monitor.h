#include "alert-buffer.h"
#include "clf.h"
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <iomanip>

using namespace std;

class HttpMonitor
{
    private:
      string m_filename;
      
      // Interval(secs): How often we want to refresh the log
      int m_interval;

      // AlertTimeRange(secs): How big the window for alerts should be
      int m_alertTimeRange;

      // StatTimeRange(secs): How big the windows for stats should be
      int m_statTimeRange;

      // AlertMin: How many requests in AlertTimeRange before alerting
      int m_alertMin;

      // Timeout (secs): How long to monitor before exiting automatically
      // -1 for infinite monitor
      int m_timeout;

      // Stats tracked per statTimeRange
      int m_totalTraffic = 0;
      int m_maxRequest = 0;
      int m_totalRequestSize = 0;
      int m_totalSuccess = 0;
      int m_totalFail = 0;

      unordered_map<string, int> m_sectionCounts;
      unordered_map<string, int> m_addrCounts;
      unordered_map<string, int> m_userCounts;

      //  UpdateStats:
      //    Updates stats tracked per statTimeRange with data
      void updateStats(CLF data);

      //  PrintStats:
      //    Prints stats tracked per statTimeRange
      void printStats();

      //  ClearStats:
      //    Clears stats tracked per statTimeRange
      void clearStats();

      // Alert:
      //  Prints alert message for hits
      static void alert(int hits);

      // CancelAlert:
      //  Prints alert cancellation message
      static void cancelAlert();

      // GetTop3:
      //  Gets top 3 elements by value from given map
      static vector<pair<string,int>> getTop3(unordered_map<string, int> &map);

      // PrintTop3:
      //  Prints top 3 elements by value from given map
      static void printTop3(string category, unordered_map<string, int> &map);

    public:
        HttpMonitor(string filename, int interval, int alertTimeRange, int statTimeRange, int alertMin, int timeout):
          m_filename(filename), m_interval(interval),
          m_alertTimeRange(alertTimeRange), m_statTimeRange(statTimeRange),
          m_alertMin(alertMin), m_timeout(timeout)
        {
        }
        // ParseLog: Main driver function
        //  Parses log given by m_filename printing out stats and alerts as necessary
        void parseLog();

        // ParseCLFLine:
        //  Creates a CLF object from parsing a string
        static CLF parseCLFLine(string line);
};