FROM alpine

RUN apk add cmake g++ git make libpcap-dev alpine-sdk linux-headers python3 tcpdump
COPY . /tra-node

WORKDIR /tra-node
RUN mkdir /tra-node/build
RUN cmake .
RUN cmake --build .