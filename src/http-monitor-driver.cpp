#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "http-monitor.h"

using namespace std;
namespace po = boost::program_options;

const int INTERVAL_DEFAULT = 1;
const int ALERT_RATE_DEFAULT = 120;
const int ALERT_THRESHHOLD_DEFAULT = 10;
const int STAT_RATE_DEFAULT = 10;

int main(int argc, char *argv[])
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "display help message")
    ("file", po::value<string>(), "HTTP access log. Default: /tmp/access.log")
    ("interval,i", po::value<int>(), "Tick rate (seconds). Default: 1 second")
    ("alertrate, ar", po::value<int>(), "Alert rate (seconds). Default: 120 second")
    ("alertthreshhold, at", po::value<int>(), "Alert threshhold (requests). Default: 10 requests")
    ("statrate, sr", po::value<int>(), "Stats rate (seconds). Default: 10 second")
  ;
  
  po::positional_options_description pDesc;
  pDesc.add("file", 1);
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(pDesc).run(), vm);
  
  // No required arguments so notify here
  po::notify(vm);
  
  if (vm.count("help"))
  {
    cout << desc << endl;
    return 1;
  }
  
  string file = vm.count("file") ? vm["file"].as<string>() : "/tmp/access.log";
  int interval = vm.count("interval") ? vm["interval"].as<int>() : INTERVAL_DEFAULT;
  int alertrate = vm.count("alertrate") ? vm["alertrate"].as<int>() : ALERT_RATE_DEFAULT;
  int alertthreshhold = vm.count("alertthreshhold") ? vm["alertthreshhold"].as<int>() : ALERT_THRESHHOLD_DEFAULT;
  int statrate = vm.count("statrate") ? vm["statrate"].as<int>() : STAT_RATE_DEFAULT;

  // Check for valid input
  if (interval < 0 || alertrate < 0 || alertthreshhold < 0 || statrate < 0 || interval > alertrate || interval > statrate)
  {
    cout << desc << endl;
    return 1;
  }

  // Run log parser
  HttpMonitor hm(file, interval, alertrate, statrate, alertthreshhold, -1 /*timeout*/);
  hm.parseLog();
  
  return 0;
}
