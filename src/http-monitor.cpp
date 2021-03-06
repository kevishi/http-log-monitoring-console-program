#include "http-monitor.h"

using namespace std;
using namespace HttpMonitorHelper;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
HttpMonitor::ParseLog:

  Main driver function

  Opens the file specfied in object and parses through any new lines
  as CLF. Then keeps track of various statistics and keeps a moving
  window for detection of high traffic. Prints out statistics and
  alerts at construction defined times.

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HttpMonitor::parseLog()
{
  // This will be called on each quantized unit of time
  // As a future consideration I'd like a more accurate measurement, but this is a good approximation

  AlertBuffer alertBuffer(m_alertTimeRange/m_interval, m_alertMin);
  ifstream logFile(m_filename);
  if (!logFile)
  {
    cout << "While opening " << m_filename << " an error occurred." << endl;
    return;
  }
  string line;
  int trafficInInterval = 0;
  int numIntervals = 0;
  int maxStatInteval = m_statTimeRange / m_interval < 1 ? 1 : m_statTimeRange / m_interval;
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
          CLF parsedLine(line);
          if (parsedLine.isValid())
          {
            trafficInInterval++;
            updateStats(parsedLine);
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
          // Requests above threshhold
          if (!inAlert)
          {
            alert(alertBuffer.getTotalTraffic());
            inAlert = true;
          }
        }
        else
        {
          // Requests not above threshhold
          if(inAlert)
          {
            cancelAlert();
            inAlert = false;
          }
        }

        // Wait one interval
        this_thread::sleep_for(chrono::seconds(m_interval));
        trafficInInterval = 0;

        // Check interval for stats
        numIntervals++;
        if (numIntervals >= maxStatInteval)
        {
          printStats();
          clearStats();
          numIntervals = 0;
        }

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
  // Only useful in timeout case
  logFile.close();
}

void HttpMonitor::updateStats(CLF data)
{
  m_totalTraffic += data.isValid();

  // top 3 sections
  string section = data.getSection();
  m_sectionCounts[section]++;

  // top 3 users
  m_userCounts[data.getUser()]++;

  // top 3 clients
  m_addrCounts[data.getAddr()]++;

  // total success/error
  m_totalSuccess += (data.getStatus() / 100 <= 3);
  m_totalFail += (data.getStatus() / 100 > 3);

  // largest/total request size
  m_maxRequest = data.getSize() > m_maxRequest ? data.getSize() : m_maxRequest;
  m_totalRequestSize += data.getSize();
}


void HttpMonitor::printStats()
{

  cout << "---- Stats for the past " << m_statTimeRange << " seconds -----" << endl;
  // top 3 sections
  printTop3("Section", m_sectionCounts);

  // top 3 users
  printTop3("User", m_userCounts);

  // top 3 clients
  printTop3("Client", m_addrCounts);

  // total success/error
  if (m_totalTraffic > 0)
  {
    float percentSuccess = (float)m_totalSuccess / (float)m_totalTraffic;
    cout << "Success%: " << percentSuccess * 100 <<"%"<< endl;
  }

  // largest/total request size
  cout << "Largest Request size: " << m_maxRequest << " bytes" << endl;
  if (m_totalTraffic > 0)
  {
    cout << "Average Request size: " << int((float)m_totalRequestSize / (float)m_totalTraffic) << " bytes" << endl;
  }
}

void HttpMonitor::clearStats()
{
  int m_totalTraffic = 0;
  int m_maxRequest = 0;
  int m_totalRequestSize = 0;
  int m_totalSuccess = 0;
  int m_totalFail = 0;

  m_sectionCounts.clear();
  m_addrCounts.clear();
  m_userCounts.clear();
}

// Helpers

// Alert:
//  Prints alert message for hits
void HttpMonitor::alert(int hits)
{
  auto now = std::chrono::system_clock::now();
  std::time_t cur_time = std::chrono::system_clock::to_time_t(now);
  cout << "High traffic generated an alert - hits = " << hits << ", triggered at " << std::ctime(&cur_time);
}

// CancelAlert:
//  Prints alert cancellation message
void HttpMonitor::cancelAlert()
{
  auto now = std::chrono::system_clock::now();
  std::time_t cur_time = std::chrono::system_clock::to_time_t(now);
  cout << "High traffic alert recovered at " << std::ctime(&cur_time);
}

// GetTop3:
//  Gets top 3 elements by value from given map
vector<pair<string,int>> HttpMonitor::getTop3(unordered_map<string, int> &map)
{
  int topk = map.size() >= 3 ? 3 : map.size();
  vector<pair<string, int>> top3(topk);
  auto comp = [](pair<string,int> l, pair<string, int> r)
  {
    return l.second > r.second;
  };
  partial_sort_copy(
    map.begin(),
    map.end(),
    top3.begin(),
    top3.end(),
    comp
  );

  return top3;
}

// PrintTop3:
//  Prints top 3 elements by value from given map
void HttpMonitor::printTop3(string category, unordered_map<string, int> &map)
{
  const char separator = ' ';
  const int width = 20;

  vector<pair<string, int>> top3 = getTop3(map);
  cout << "Top " << category << "s:" << endl <<
       left << setw(width) << setfill(separator) << category <<
       left << setw(width) << setfill(separator) << "Requests" << endl;
  int n = map.size() > 3 ? 3 : map.size();
  for (int i = 1; i <= n; i++)
  {
    cout << i << ". " <<
    left << setw(width) << setfill(separator) << top3[i-1].first <<
    left << setw(width) << setfill(separator) << top3[i-1].second << endl;
  }
}
// End Helpers