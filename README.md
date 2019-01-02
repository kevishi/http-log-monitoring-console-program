# HTTP Log Monitoring Console Program
Consumes an actively written-to w3c-formatted HTTP access log (https://www.w3.org/Daemon/User/Config/Logging.html).

Usage:

  -h [ --help ]                display help message
  
  --file arg                   HTTP access log. Default: /tmp/access.log
  
  -i [ --interval ] arg        Tick rate (seconds). Default: 1 second
  
  -r  [ --alertrate ] arg       Alert rate (seconds). Default: 120 second
  
  -t  [ --alertthreshhold ] arg Alert threshhold (requests). Default: 10 requests
  
  -s  [ --statrate ] arg        Stats rate (seconds). Default: 10 second
  
Arguments less than or equal to 0 is undefined behavior.
Intervals being longer than the alert rate or stats rate is also undefined behavior.

Displays stats every Stats Rate seconds. Currently includes:
1. Top k sections
2. Top k users
3. Top k clients
4. Success/Fail request percentage
5. Largest/Average request size

Any time the number of requests in Alert Rate seconds is above Alert Threshhold displays an alert

# Requirements
gcc, libboost-all-dev

# Usage
Run ./build.sh
Run ./http-monitor

# Docker
Currently the dockerfile has entrypoint "./http-monitor". This will default to the /tmp/access.log of the docker image. It is strongly recommended you ssh into the docker container to edit the log manually for testing.

The dockerfile automatically runs its tests.
