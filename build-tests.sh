gcc -o http-monitor-tests tests/*.cpp src/traffic-stat-buffer.cpp src/alert-buffer.cpp src/http-monitor.cpp -lboost_program_options -lstdc++ -lpthread
