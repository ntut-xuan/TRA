#ifndef GTP_U_IDENTIFIER_HPP
#define GTP_U_IDENTIFIER_HPP

#include "spdlog/spdlog.h"
#include "tins/ip.h"
#include "tins/pdu.h"
#include "tins/rawpdu.h"
#include "tins/udp.h"
#include <string>

inline int is_gtpu_packet(Tins::UDP *udp) { return udp->sport() == 2152 && udp->dport() == 2152; }

inline int extract_gtpu_packet_identification(Tins::UDP *udp) {
    std::vector<uint8_t> vec = udp->rfind_pdu<Tins::RawPDU>().payload();
    Tins::RawPDU rawpdu(vec.data(), vec.size());

    std::vector<uint8_t> GPRS_header = std::vector<uint8_t>(vec.begin(), vec.end() + 16);
    std::vector<uint8_t> IPv4_header = std::vector<uint8_t>(vec.begin() + 16, vec.end() + 36);

    Tins::IP ip(IPv4_header.data(), IPv4_header.size());

    return ip.id();
}

#endif