#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <chrono>
#include <thread>

#include "cyclic-buffer.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "display help message")
    ("file", po::value<string>(), "HTTP access log")
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
  
  if (vm.count("file"))
  {
    //TODO change this
    for(int i = 0; i < 10; i++)
    {
      // character code for celarning screen (same behavior as clear)
      // I would want to extend this using ncurses with mroe time
      cout << "\x1B[2J\x1B[H";
      cout << vm["file"].as<string>() << i << endl;
      this_thread::sleep_for(chrono::seconds(1));
    }
  }
  
  return 0;
}
