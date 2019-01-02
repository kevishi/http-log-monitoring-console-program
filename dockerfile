FROM gcc:8.2
RUN apt-get update
RUN apt-get install -y libboost-all-dev

COPY tests/ /workdir/tests
COPY src/ /workdir/src
COPY build.sh /workdir/build.sh
COPY build-tests.sh /workdir/build-tests.sh
WORKDIR /workdir

RUN ./build.sh
RUN ./build-tests.sh
RUN ./http-monitor-tests