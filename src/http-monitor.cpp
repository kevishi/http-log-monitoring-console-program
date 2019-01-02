#include "http-monitor.h"

using namespace std;
using namespace HttpMonitorHelper;

CLF HttpMonitor::parseCLFLine(string line)
{
  CLF data;
  data.isValid = true;
  
  size_t addr = line.find(" - ");
  if (addr != string::npos)
  {
    data.addr = line.substr(0, addr);
  }
  else
  {
    data.isValid = false;
  }

  size_t user = line.find(" [", addr + 3);
  if (user != string::npos)
  {
    data.user = line.substr(addr + 3, user - (addr + 3));
  }
  else
  {
    data.isValid = false;
  }

  size_t ltime = line.find("] \"", user + 2);
  if (ltime != string::npos)
  {
    data.time = line.substr(user + 2, ltime - (user + 2));
  }
  else
  {
    data.isValid = false;
  }

  size_t req = line.find("\" ", ltime + 3);
  if (req != string::npos)
  {
    data.request = line.substr(ltime + 3, req - (ltime + 3));
  }
  else
  {
    data.isValid = false;
  }

  size_t status = line.find(' ', req + 2);
  if (status != string::npos)
  {
    try
    {
      data.status = stoi(line.substr(req + 2, status - (req + 2)));
      data.size = stoi(line.substr(status + 1, line.size() - (status + 1)));
    }
    catch(...)
    {
      data.isValid = false;
    }
  }
  else
  {
    data.isValid = false;
  }

  return data;
}

void alert(int hits)
{
  auto now = std::chrono::system_clock::now();
  std::time_t cur_time = std::chrono::system_clock::to_time_t(now);
  cout << "High traffic generated an alert - hits = " << hits << ", triggered at " << std::ctime(&cur_time);
}

void cancelAlert()
{
  auto now = std::chrono::system_clock::now();
  std::time_t cur_time = std::chrono::system_clock::to_time_t(now);
  cout << "High traffic alert recovered at " << std::ctime(&cur_time);
}

void HttpMonitor::parseLog()
{
  // This will be called on each quantized unit of time
  // As a future consideration I'd like a more accurate measurement, but this is a good approximation

  AlertBuffer alertBuffer(m_alertTimeRange/m_interval, m_alertMin);
  ifstream logFile(m_filename);
  string line;
  int trafficInInterval = 0;
  bool inAlert = false;
  
  // ASSUMPTION: This logger is time agnostic and only cares about new log entries after file open
  // Parse to end of file
  while (getline(logFile, line));
  
  auto start = chrono::steady_clock::now();
  while (true) {

      // We assume parsing is fast enough that timeToParse << interval so we simply wait interval after parsing
      // I would want to extend this to be able to be safely interrupted with a key
      // if (getline(&line, &len, fp) != -1)
      if (getline(logFile, line))
      {
        if (line.size() > 0)
        {
          CLF parsedLine = parseCLFLine(line);
          if (parsedLine.isValid)
          {
            trafficInInterval++;
          }
        }
      }
      else
      {
        // Reached EOF for last interval
        // Update stats/alert for last interval's traffic
        AlertUnit alertIntervalUnit;
        alertIntervalUnit.totalTraffic = trafficInInterval;
        if (alertBuffer.insert(alertIntervalUnit))
        {
          if (!inAlert)
          {
            alert(alertBuffer.getTotalTraffic());
            inAlert = true;
          }
        }
        else
        {
          if(inAlert)
          {
            cancelAlert();
            inAlert = false;
          }
        }

        // Wait one interval
        this_thread::sleep_for(chrono::seconds(m_interval));
        trafficInInterval = 0;

        // Refresh file buffer
        logFile.clear();
        
        // Check timeout
        auto end = chrono::steady_clock::now();
        if(m_timeout != -1 && chrono::duration_cast<chrono::seconds>(end - start).count() > m_timeout)
        {
          break;
        }
      }
  }
  // Cleanup code that I would use if I was using polling
  logFile.close();
}
