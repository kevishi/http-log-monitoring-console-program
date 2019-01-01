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
};

class HttpMonitor
{
    string m_filename;
    int m_interval;
    public:
        HttpMonitor(string filename, int interval): m_filename(filename), m_interval(interval) {}
        void parseLog();
        static CLF parseCLFLine(string line);
};