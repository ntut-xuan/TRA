FROM alpine

RUN apk add cmake g++ git make libpcap-dev alpine-sdk linux-headers python3 tcpdump ninja
COPY . /tra-node

WORKDIR /tra-node
RUN mkdir ./build
RUN cmake -Bbuild . -GNinja
RUN cmake --build ./build

CMD ./build/TRA $INPUT $OUTPUT $UPF_N4_IP $CONTROLLER_IP