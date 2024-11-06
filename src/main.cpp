#include "tins/ethernetII.h"
#include "tins/ip.h"
#include <cstdio>
#include <iomanip>
#include <string>
#include <tins/tins.h>
#include <iostream>
#include <chrono>
#include <sstream>

#include "../include/protocol.h"
#include "tins/packet.h"
#include "tins/sniffer.h"
#include "tins/tcp.h"
#include "tins/timestamp.h"

#include <date/date.h>

using namespace std::chrono;

int main(){
    Tins::Sniffer sniffer("en0");

    auto start_time = system_clock::now();
    auto end_time = start_time + seconds(5);

    sniffer.sniff_loop([&](Tins::PDU& pdu) -> bool {
        auto current_time = system_clock::now();
        auto current_time_nanosecond = date::floor<nanoseconds>(current_time);
        if(current_time <= end_time){
            Tins::IP packet = pdu.rfind_pdu<Tins::IP>();
            char buf[256];
            snprintf(buf, 256, "%s %s(%d) / Source IP %s / Destination IP %s / Identification %d",
                date::format("%Y-%m-%d %T", current_time_nanosecond).c_str(),
                convert_protocol_id_to_string(packet.protocol()).c_str(),
                packet.protocol(),
                packet.src_addr().to_string().c_str(),
                packet.dst_addr().to_string().c_str(),
                packet.id()
            );
            std::cout << std::string(buf) << std::endl;
            return true;
        }
        return false;
    });
}