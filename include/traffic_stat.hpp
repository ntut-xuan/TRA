#ifndef TRAFFIC_STAT_HPP
#define TRAFFIC_STAT_HPP

#include "spdlog/spdlog.h"
#include "traffic_data.hpp"

class TrafficStat {
  private:
    int cpu_usage;
    long long total_delay_time_in_nanosecond;
    std::set<TrafficData> receive_set;
    std::set<TrafficData> waiting_set;

  public:
    void add_request_packet(TrafficData data) {
        receive_set.insert(data);
        waiting_set.insert(data);
    }
    void add_transmit_packet(TrafficData data) {
        auto req_packet = waiting_set.find(data);
        if (waiting_set.find(data) != waiting_set.end()) {
            total_delay_time_in_nanosecond += data.get_time_in_nanoseconds() - req_packet->get_time_in_nanoseconds();
        }
        waiting_set.erase(data);
    }
    void handle_packet(TrafficData data) {
        if (is_received_packet(data)) {
            spdlog::debug("[RESP] Receive response with identification {0} / "
                          "Waiting Packet {1} / Timestamp {2}",
                          data.get_identification(), this->get_wait_packet(), data.get_time_in_nanoseconds() / 1e9);
            add_transmit_packet(data);
        } else {
            spdlog::debug("[REQ] Receive request with identification {0} / Timestamp {1}", data.get_identification(),
                          data.get_time_in_nanoseconds() / 1e9);
            add_request_packet(data);
        }
    }
    bool is_received_packet(TrafficData data) {
        if (waiting_set.find(data) != waiting_set.end()) {
            return true;
        }
        return false;
    }
    double get_packet_loss() { return waiting_set.size() * 1.0 / receive_set.size(); }
    double get_queueing_delay_in_nanosecond() { return total_delay_time_in_nanosecond * 1.0 / receive_set.size(); }
    int get_wait_packet() { return waiting_set.size(); }
    int get_received_packet() { return receive_set.size(); }
    int get_transmitted_packet() { return receive_set.size() - waiting_set.size(); }
};

#endif