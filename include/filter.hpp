#ifndef FILTER_HPP
#define FILTER_HPP

#include "tins/icmp.h"
#include "tins/packet.h"
#include "tins/tcp.h"
#include "tins/udp.h"

enum PacketType {
    GTPU,
    TCP,
    UDP,
    ICMP,
    UNSUPPORT
};

inline bool is_gtp_u_packet(Tins::Packet &packet){
    if(packet.pdu()->find_pdu<Tins::UDP>()){
        const Tins::UDP* udp = packet.pdu()->find_pdu<Tins::UDP>();
        uint16_t dst_port = udp->dport();
        uint16_t src_port = udp->sport();

        return (dst_port == 2152 || src_port == 2152);
    }
    return false;
}

inline bool is_tcp(Tins::Packet &packet){
    if(packet.pdu()->find_pdu<Tins::TCP>()){
        return true;
    }
    return false;
}

inline bool is_udp(Tins::Packet &packet){
    if(packet.pdu()->find_pdu<Tins::UDP>()){
        return true;
    }
    return false;
}

inline bool is_icmp(Tins::Packet &packet){
    if(packet.pdu()->find_pdu<Tins::ICMP>()){
        return true;
    }
    return false;
}

inline PacketType get_packet_type(Tins::Packet &packet){
    if(is_gtp_u_packet(packet)){
        return PacketType::GTPU;
    }
    if(is_tcp(packet)){
        return PacketType::TCP;
    }
    if(is_udp(packet)){
        return PacketType::UDP;
    }
    if(is_icmp(packet)){
        return PacketType::ICMP;
    }
    return PacketType::UNSUPPORT;
}

#endif