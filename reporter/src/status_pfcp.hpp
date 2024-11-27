#ifndef STATUS_PFCP_HPP
#define STATUS_PFCP_HPP

#include <cstdint>
#include <vector>

class StatusPFCP {
  private:
    uint32_t ip_address;
    uint32_t cpu_usage;
    double packet_loss;
    double queueing_delay;

  public:
    StatusPFCP(std::vector<uint8_t> packet) {
        ip_address = ((uint32_t)packet[13] << 24) | ((uint32_t)packet[14] << 16) | ((uint32_t)packet[15] << 8) |
                     ((uint32_t)packet[16]);
        cpu_usage = ((uint32_t)packet[17] << 8) | ((uint32_t)packet[18]);
        packet_loss = (((uint32_t)packet[19] << 24) | ((uint32_t)packet[20] << 16) | ((uint32_t)packet[21] << 8) |
                       ((uint32_t)packet[22] << 8)) +
                      (((uint32_t)packet[23] << 24) | ((uint32_t)packet[24] << 16) | ((uint32_t)packet[25] << 8) |
                       ((uint32_t)packet[26] << 8)) /
                          1e6;
        queueing_delay = (((uint32_t)packet[27] << 24) | ((uint32_t)packet[28] << 16) | ((uint32_t)packet[29] << 8) |
                          ((uint32_t)packet[30] << 8)) +
                         (((uint32_t)packet[31] << 24) | ((uint32_t)packet[32] << 16) | ((uint32_t)packet[33] << 8) |
                          ((uint32_t)packet[34] << 8)) /
                             1e6;
    }
    uint32_t get_ip_address() { return ip_address; }
    uint32_t get_cpu_usage() { return cpu_usage; }
    double get_packet_loss() { return packet_loss; }
    double get_queueing_delay() { return queueing_delay; }
};

#endif