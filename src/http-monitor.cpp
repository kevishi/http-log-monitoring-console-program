#include "http-monitor.h"

using namespace std;

CLF HttpMonitor::parseCLFLine(string line)
{
  CLF data;
  size_t addr = line.find(" - ");
  if (addr != string::npos)
  {
    data.addr = line.substr(0, addr);
  }

  size_t user = line.find(" [", addr + 3);
  if (user != string::npos)
  {
    data.user = line.substr(addr + 3, user - (addr + 3));
  }

  size_t ltime = line.find("] \"", user + 2);
  if (ltime != string::npos)
  {
    data.time = line.substr(user + 2, ltime - (user + 2));
  }

  size_t req = line.find("\" ", ltime + 3);
  if (req != string::npos)
  {
    data.request = line.substr(ltime + 3, req - (ltime + 3));
  }

  size_t status = line.find(' ', req + 2);
  if (status != string::npos)
  {
    data.status = stoi(line.substr(req + 2, status - (req + 2)));
  }

  data.size = stoi(line.substr(status + 1, line.size() - (status + 1)));

  return data;
}

void HttpMonitor::parseLog()
{
  // C code for faster performance
  // This will be called on each quantized unit of time, so performance will help limit the desynchronization
  // As a future consideration I'd like a more accurate measurement, but this is a good approximation
  FILE* fp = fopen(m_filename.c_str(), "r");
  if (fp == NULL)
      exit(1);

  char* line = NULL;
  size_t len = 0;
  while ((getline(&line, &len, fp)) != -1) {
      // using printf() in all tests for consistency
      printf("%s", line);
  }
  fclose(fp);
  if (line)
      free(line);
}
