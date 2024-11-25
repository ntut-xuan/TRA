# TRA (Traffic Realtime Analysis)

TRA (Traffic Realtime Analysis) is an application to fetch packet loss and queueing delay between two network interface.

With TRA, it can sniff receive packet and transmit packet and check the packet is transmit through your service successfully or not.

## Installation

TRA need `libpcap` to build, please make sure that you already have `libpcap-dev` first.

```
cmake .
cmake --build
```

## Dev Environment

We provide a docker for build environment. It provide two test device and already configure the tra-node as the gateway.

```
+------------+            +------------+            +------------+
+            +            +            +            +            +
+  DEVICE-1  + ---------- +  TRA-NODE  + ---------- +  DEVICE-2  +
+            +            +            +            +            +
+------------+            +------------+            +------------+
```

We expected that `DEVICE-1` and `DEVICE-2` will communicate through `TRA-NODE`. `TRA-NODE` will sniff two interface which connect `DEVICE-1` and `DEVICE-2`.

Since `TRA-NODE` is play as gateway, we expected that the identification value of received packet of `TRA-NODE` will as same as transmit packet from `TRA-NODE`. Therefore, we can check every packet `TRA-NODE` received is transmit successfully or not.

It's good to develop TRA with this environment to test TRA is work or not.