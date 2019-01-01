#include "alert-buffer.h"
#include <string>

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
        void parseLog();
        static CLF parseCLFLine(string line);
};