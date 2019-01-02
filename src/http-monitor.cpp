#include "http-monitor.h"

using namespace std;
using namespace HttpMonitorHelper;

// Helpers
// ParseCLFLine:
//  Creates a CLF object from parsing a string
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

// Alert:
//  Prints alert message for hits
void alert(int hits)
{
  auto now = std::chrono::system_clock::now();
  std::time_t cur_time = std::chrono::system_clock::to_time_t(now);
  cout << "High traffic generated an alert - hits = " << hits << ", triggered at " << std::ctime(&cur_time);
}

// CancelAlert:
//  Prints alert cancellation message
void cancelAlert()
{
  auto now = std::chrono::system_clock::now();
  std::time_t cur_time = std::chrono::system_clock::to_time_t(now);
  cout << "High traffic alert recovered at " << std::ctime(&cur_time);
}

// GetSection:
//  Gets section from CLF
string getSection(CLF data)
{
  int posSection = data.request.find("/");
  int posNextSection = data.request.find("/", posSection+1);
  int posNextRequestPart = data.request.find(" ", posSection+1);
  int posSectionEnd = posNextSection > posNextRequestPart ? posNextRequestPart : posNextSection;
  if(posSection != string::npos && posSectionEnd != string::npos)
  {
    return data.request.substr(posSection, posSectionEnd-posSection);
  }

  return "";
}

// GetTop3:
//  Gets top 3 elements by value from given map
vector<pair<string,int>> getTop3(unordered_map<string, int> &map)
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
// End Helpers


void HttpMonitor::updateStats(CLF data)
{
  m_totalTraffic += data.isValid;

  // top 3 sections
  string section = getSection(data);
  m_sectionCounts[section]++;

  // top 3 users
  m_userCounts[data.user]++;

  // top 3 clients
  m_addrCounts[data.addr]++;

  // total success/error
  m_totalSuccess += (data.status / 100 <= 3);
  m_totalFail += (data.status / 100 > 3);

  // largest/total request size
  m_maxRequest = data.size > m_maxRequest ? data.size : m_maxRequest;
  m_totalRequestSize += data.size;
}


void HttpMonitor::printStats()
{
    const char separator = ' ';
    const int width = 20;

  cout << "---- Stats for the past " << m_statTimeRange << " seconds -----" << endl;
  // top 3 sections
  vector<pair<string, int>> top3sections = getTop3(m_sectionCounts);
  cout << "Top sections:" << endl <<
       left << setw(width) << setfill(separator) << "Section " <<
       left << setw(width) << setfill(separator) << " Requests" << endl;
  int n = m_sectionCounts.size() > 3 ? 3 : m_sectionCounts.size();
  for (int i = 1; i <= n; i++)
  {
    cout << i << ". " <<
    left << setw(width) << setfill(separator) << top3sections[i-1].first <<
    left << setw(width) << setfill(separator) << top3sections[i-1].second << endl;
  }

  // top 3 users
  vector<pair<string, int>> top3users = getTop3(m_userCounts);
  cout << "Top sections:" << endl <<
       left << setw(width) << setfill(separator) << "User " <<
       left << setw(width) << setfill(separator) << " Requests" << endl;
  n = m_userCounts.size() > 3 ? 3 : m_userCounts.size();
  for (int i = 1; i <= n; i++)
  {
    cout << i << ". " <<
    left << setw(width) << setfill(separator) << top3users[i-1].first <<
    left << setw(width) << setfill(separator) << top3users[i-1].second << endl;
  }

  // top 3 clients
  vector<pair<string, int>> top3clients = getTop3(m_addrCounts);
  cout << "Top sections:" << endl <<
       left << setw(width) << setfill(separator) << "Client " <<
       left << setw(width) << setfill(separator) << " Requests" << endl;
  n = m_addrCounts.size() > 3 ? 3 : m_addrCounts.size();
  for (int i = 1; i <= n; i++)
  {
    cout << i << ". " <<
    left << setw(width) << setfill(separator) << top3clients[i-1].first <<
    left << setw(width) << setfill(separator) << top3clients[i-1].second << endl;
  }

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

void HttpMonitor::parseLog()
{
  // This will be called on each quantized unit of time
  // As a future consideration I'd like a more accurate measurement, but this is a good approximation

  AlertBuffer alertBuffer(m_alertTimeRange/m_interval, m_alertMin);
  ifstream logFile(m_filename);
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
          CLF parsedLine = parseCLFLine(line);
          if (parsedLine.isValid)
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

        // Keep interval for stats
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
  // Cleanup code that I would use if I was using polling
  logFile.close();
}
