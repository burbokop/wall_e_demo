# Build

FROM gcc:latest as build

RUN apt-get update
RUN apt-get install -y python-pip
RUN pip install cmake

RUN apt-get install -y qtbase5-dev
RUN apt-get install -y qtdeclarative5-dev

WORKDIR /app

ADD ./src ./src
ADD ./wall_e ./wall_e
ADD ./resources ./resources
ADD ./resources.qrc ./
ADD ./CMakeLists.txt ./

WORKDIR /app/build
RUN cmake .. && make

# Run

FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y libqt5core5a
RUN apt-get install -y libqt5quick5

WORKDIR /app

COPY --from=build /app/build/*.so ./bin/
COPY --from=build /app/build/km2ide ./bin/
WORKDIR /app/bin

RUN groupadd -r user && useradd -r -g user user
USER user

#ENTRYPOINT ["/bin/gdb", "-ex", "run", "./project172"]
ENTRYPOINT ["./km2ide"]
