#include "clf.h"

using namespace std;

// CLF(string)
// Constructs a CLF from a w3c-format line
CLF::CLF(string line)
{
  m_isValid = true;
  
  size_t addr = line.find(" - ");
  if (addr != string::npos)
  {
    m_addr = line.substr(0, addr);
  }
  else
  {
    m_isValid = false;
  }

  size_t user = line.find(" [", addr + 3);
  if (user != string::npos)
  {
    m_user = line.substr(addr + 3, user - (addr + 3));
  }
  else
  {
    m_isValid = false;
  }

  size_t ltime = line.find("] \"", user + 2);
  if (ltime != string::npos)
  {
    m_time = line.substr(user + 2, ltime - (user + 2));
  }
  else
  {
    m_isValid = false;
  }

  size_t req = line.find("\" ", ltime + 3);
  if (req != string::npos)
  {
    m_request = line.substr(ltime + 3, req - (ltime + 3));
  }
  else
  {
    m_isValid = false;
  }

  size_t status = line.find(' ', req + 2);
  if (status != string::npos)
  {
    try
    {
      m_status = stoi(line.substr(req + 2, status - (req + 2)));
      m_size = stoi(line.substr(status + 1, line.size() - (status + 1)));
    }
    catch(...)
    {
      m_isValid = false;
    }
  }
  else
  {
    m_isValid = false;
  }
}

// GetSection:
//  Gets section from CLF
//  A section defined as '/foo/bar' => '/foo'
string CLF::getSection()
{
  int posSection = m_request.find("/");
  int posNextSection = m_request.find("/", posSection+1);
  int posNextRequestPart = m_request.find(" ", posSection+1);
  int posSectionEnd = posNextSection > posNextRequestPart ? posNextRequestPart : posNextSection;
  if(posSection != string::npos && posSectionEnd != string::npos)
  {
    return m_request.substr(posSection, posSectionEnd-posSection);
  }

  return "";
}