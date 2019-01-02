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
  int posSectionEnd = data.request.find("/", posSection+1);
  if(posSection != string::npos && posSectionEnd != string::npos)
  {
    return data.request.substr(posSection, posSectionEnd-posSection);
  }

  return "";
}

vector<pair<string,int>> getTop3(unordered_map<string, int> &map)
{
  vector<pair<string, int>> top3(3);
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
  m_totalSuccess += (data.status % 100 <= 3);
  m_totalFail += (data.status % 100 > 3);

  // largest/total request size
  m_maxRequest = data.size > m_maxRequest ? data.size : m_maxRequest;
  m_totalRequestSize += data.size;
}


void HttpMonitor::printStats()
{
  cout << "---- Stats for the past " << m_statTimeRange << " seconds -----" << endl;
  // top 3 sections
  vector<pair<string, int>> top3sections = getTop3(m_sectionCounts);
  cout << "Top 3 sections:" << endl
       << "Section \t\t\t Requests" << endl
       << "1. " << top3sections[0].first << "\t\t\t" << top3sections[0].second << endl
       << "2. " << top3sections[1].first << "\t\t\t" << top3sections[1].second << endl
       << "3. " << top3sections[2].first << "\t\t\t" << top3sections[2].second << endl;

  // top 3 users
  vector<pair<string, int>> top3users = getTop3(m_userCounts);
  cout << "Top 3 users:" << endl
       << "User \t\t\t Requests" << endl
       << "1. " << top3users[0].first << "\t\t\t" << top3users[0].second << endl
       << "2. " << top3users[1].first << "\t\t\t" << top3users[1].second << endl
       << "3. " << top3users[2].first << "\t\t\t" << top3users[2].second << endl;

  // top 3 clients
  vector<pair<string, int>> top3clients = getTop3(m_addrCounts);
  cout << "Top 3 clients:" << endl
       << "Client \t\t\t Requests" << endl
       << "1. " << top3clients[0].first << "\t\t\t" << top3clients[0].second << endl
       << "2. " << top3clients[1].first << "\t\t\t" << top3clients[1].second << endl
       << "3. " << top3clients[2].first << "\t\t\t" << top3clients[2].second << endl;

  // total success/error
  cout << "Success %:" <<  <<"%"<< endl;

  // largest/total request size
  cout << "Largest Request size: " << << " bytes" << endl;
  cout << "Average Request size: " << << " bytes" << endl;
}

void HttpMonitor::clearStats()
{

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
  int maxStatInteval = m_statTimeRange / numIntervals < 1 ? 1 : m_statTimeRange / numIntervals;
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
