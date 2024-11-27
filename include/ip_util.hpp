#ifndef IP_UTIL_HPP
#define IP_UTIL_HPP

#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>
#include <string>

static uint32_t convert_ip_str_to_uint32_t(std::string ip) {
    in_addr addr;
    inet_pton(AF_INET, ip.c_str(), &addr);
    return addr.s_addr;
}

static std::string convert_uint32t_ip_to_str(uint32_t ip) {
    char buf[16];
    auto result = inet_ntop(AF_INET, &ip, buf, sizeof(buf));
    return std::string(buf);
}

#endif