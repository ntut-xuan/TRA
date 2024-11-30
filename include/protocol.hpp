#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>

inline std::string convert_ipv4_protocol_to_string(int protocol) {
    if (protocol == 1) {
        return "ICMP";
    }
    if (protocol == 6) {
        return "TCP";
    }
    if (protocol == 17) {
        return "UDP";
    }
    return "UNKNOWN";
}

#endif