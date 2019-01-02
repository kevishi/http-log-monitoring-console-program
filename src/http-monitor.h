#include "alert-buffer.h"
#include <string>
#include <chrono>
#include <thread>
#include <fstream>

using namespace std;

struct CLF
{
  string addr;
  string user;
  string time;
  string request;
  int status;
  int size;

  bool isValid;
};

class HttpMonitor
{
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

    public:
        HttpMonitor(string filename, int interval, int alertTimeRange, int statTimeRange, int alertMin, int timeout):
          m_filename(filename), m_interval(interval),
          m_alertTimeRange(alertTimeRange), m_statTimeRange(statTimeRange),
          m_alertMin(alertMin), m_timeout(timeout)
        {
        }
        void parseLog();
        static CLF parseCLFLine(string line);
};