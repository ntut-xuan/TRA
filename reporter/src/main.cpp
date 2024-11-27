#include "sockpp/result.h"
#include "sockpp/sock_address.h"
#include <sockpp/socket.h>
#include <sockpp/udp_socket.h>

#include "reporter_socket.hpp"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        spdlog::error("Error: need host.");
        spdlog::error("Usage: ./reporter <HOST>");
        return -1;
    }

    std::string host = argv[1];

    sockpp::initialize();

    sockpp::result<> res = sock.bind(sockpp::inet_address(host, 8805));

    if (res.is_error()) {
        spdlog::error("Error: Failed to bind address {0} with port 8805.", host);
        return -1;
    }

    sockpp::result<size_t> recv_result;
    uint8_t buf[512];
    typename sockpp::udp_socket::addr_t srcAddr;

    while ((recv_result = sock.recv_from(buf, sizeof(buf), &srcAddr)).value() > 0) {
        spdlog::info("[REPORT] Packet Size {0}", recv_result.value());
    }

    return 0;
}