#ifndef TRAFFIC_STAT_HPP
#define TRAFFIC_STAT_HPP

#include "config.hpp"
#include "cpu.hpp"
#include "ip_util.hpp"
#include "spdlog/spdlog.h"
#include "traffic_data.hpp"
#include <climits>

class TrafficStat {
  private:
    uint32_t upfn4ip = 0;
    int timestamp = 0;
    long long total_delay_time_in_nanosecond = 0;
    std::set<TrafficData> receive_set;
    std::set<TrafficData> waiting_set;

  public:
    TrafficStat() : upfn4ip(0), timestamp(0) {
        this->upfn4ip = convert_ip_str_to_uint32_t(ConfigSingleton::get_upf_n4_ip());
    }
    TrafficStat(uint32_t upfn4ip, int timestamp) {
        this->upfn4ip = upfn4ip;
        this->timestamp = timestamp;
    }
    void add_request_packet(TrafficData data) {
        receive_set.insert(data);
        waiting_set.insert(data);
    }
    void add_transmit_packet(TrafficData data) {
        auto req_packet = waiting_set.find(data);
        if (waiting_set.find(data) != waiting_set.end()) {
            total_delay_time_in_nanosecond +=
                std::abs(data.get_time_in_nanoseconds() - req_packet->get_time_in_nanoseconds());
        }
        waiting_set.erase(data);
    }
    void handle_packet(TrafficData data) {
        if (is_received_packet(data)) {
            spdlog::debug("[RESP] Receive response with identification {0} / "
                          "Waiting Packet {1} / Timestamp {2} / Protocol {3}({4}) / Source IP {5} / Destination IP {6}",
                          data.get_identification(), this->get_wait_packet(), data.get_time_in_nanoseconds() / 1e9);
            add_transmit_packet(data);
        } else {
            if (data.get_source_ip() == "192.168.43.21") {
                return;
            }
            spdlog::debug("[REQ] Receive request with identification {0} / Timestamp {1} / "
                          "Protocol {3}({2}) / Source IP {4} / Destination IP {5}",
                          data.get_identification(), data.get_time_in_nanoseconds() / 1e9, data.get_protocol(),
                          data.get_protocol_name(), data.get_source_ip().to_string(),
                          data.get_destination_ip().to_string());
            add_request_packet(data);
        }
    }
    bool is_received_packet(TrafficData data) {
        if (waiting_set.find(data) != waiting_set.end()) {
            return true;
        }
        return false;
    }
    double get_packet_loss() {
        if (receive_set.size() == 0) {
            return 0;
        }
        return waiting_set.size() * 1.0 / receive_set.size();
    }
    double get_queueing_delay_in_nanosecond() {
        if (receive_set.size() == 0) {
            return 0;
        }
        return total_delay_time_in_nanosecond * 1.0 / receive_set.size();
    }
    int get_wait_packet() { return waiting_set.size(); }
    int get_received_packet() { return receive_set.size(); }
    int get_transmitted_packet() { return receive_set.size() - waiting_set.size(); }
    int get_upf_n4_ip() { return upfn4ip; }
    int get_timestamp() { return timestamp; }
};

#endif