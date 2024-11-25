#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>

inline std::string convert_protocol_id_to_string(uint8_t id) {
    if (id == 1) {
        return "ICMP";
    }
    if (id == 6) {
        return "TCP";
    }
    if (id == 17) {
        return "UDP";
    }

    return "UNKNOWN";
}

#endif