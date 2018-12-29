FROM gcc:8.2
COPY src/ /workdir/src
WORKDIR /workdir
RUN gcc -lstdc++ -o http-monitor src/main.cpp
RUN ./http-monitor