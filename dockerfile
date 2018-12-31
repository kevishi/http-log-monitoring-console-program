FROM gcc:8.2
COPY src/ /workdir/src
WORKDIR /workdir
RUN gcc -lstdc++ -o http-monitor src/http-monitor.cpp -lboost_program_options
RUN ./http-monitor