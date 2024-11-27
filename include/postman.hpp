#ifndef POSTMAN_HPP
#define POSTMAN_HPP

#include "pfcp.hpp"
#include "spdlog/spdlog.h"
#include "tins/ip.h"
#include "tins/packet_sender.h"
#include "tins/rawpdu.h"
#include "traffic_stat.hpp"
#include <cstdint>

inline static void submit_report(TrafficStat stat, std::string dest_ip, int port) {
    std::vector<uint8_t> pfcp_stat_packet = convert_traffic_stat_into_pfcp_packet(stat);
    const unsigned char *buf = pfcp_stat_packet.data();

    Tins::PacketSender sender;
    Tins::IP pkt = Tins::IP(dest_ip) / Tins::UDP(port) / Tins::RawPDU(buf, pfcp_stat_packet.size());

    sender.send(pkt);

    spdlog::info("[SUBMIT] Send traffic report / Timestamp {0} / UPF N4 IP {1} / Packet Loss {2} / Queueing Delay {3} "
                 "/ CPU {4}%",
                 stat.get_timestamp(), convert_uint32t_ip_to_str(stat.get_upf_n4_ip()), stat.get_packet_loss(),
                 stat.get_queueing_delay_in_nanosecond() / 1e6, stat.get_cpu_usage());
}

#endif