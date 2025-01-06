# TRA (Traffic Realtime Analysis)

> We call it TRA just because the name is coincides with [Taiwan Railway Administration（臺鐵）](https://en.wikipedia.org/wiki/Taiwan_Railways_Administration) and it's fun.
>
> The packet may delay but it will reach the destination, LOL.

![TRA](https://github.com/user-attachments/assets/8280af8a-193d-44ea-b4f5-d5c19bb62226)

TRA (Traffic Realtime Analysis) is an application to fetch packet loss and queueing delay between two network interface.

With TRA, it can sniff receive packet and transmit packet and check the packet is transmit through your service successfully or not.

## OS Requirement

TRA only support on Linux with `libpcap` library. Please take a look about [Installation](#Installation) part.

## Installation

TRA need `libpcap` to build, please make sure that you already have `libpcap-dev` first.

```
cmake .
cmake --build
```

## Dev Environment

We provide a docker for build environment. It provide two test device and already configure the tra-node as the gateway.

```
+------------------------+                          +------------------------+
|                   LAN1 |                          | LAN2                   |
|    +------------+      |      +------------+      |      +------------+    |
|    +            +      |      +            +      |      +            +    |
|    +  DEVICE-1  + -----|----- +  TRA-NODE  + -----|----- +  DEVICE-2  +    |
|    +            +      |      +            +      |      +            +    |
|    +------------+      |      +------------+      |      +------------+    |
|                        |                          |                        |
+------------------------+                          +------------------------+
```

We expected that `DEVICE-1` and `DEVICE-2` will communicate through `TRA-NODE`. `TRA-NODE` will sniff two interface which connect `DEVICE-1` and `DEVICE-2`.

Since `TRA-NODE` is play as gateway, we expected that the identification value of received packet of `TRA-NODE` will as same as transmit packet from `TRA-NODE`. Therefore, we can check every packet `TRA-NODE` received is transmit successfully or not.

It's good to develop TRA with this environment to test TRA is work or not.
