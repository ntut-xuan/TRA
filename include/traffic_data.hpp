#ifndef TRAFFIC_DATA_H
#define TRAFFIC_DATA_H

#include <tins/tins.h>

class TrafficData {
  private:
    long long nanoseconds;
    int type;
    Tins::IPv4Address source_ip;
    Tins::IPv4Address destination_ip;
    int identification;

  public:
    TrafficData() = default;
    TrafficData(long long nanoseconds, int type, Tins::IPv4Address source_ip, Tins::IPv4Address destination_ip,
                int identification)
        : nanoseconds(nanoseconds), type(type), source_ip(source_ip), destination_ip(destination_ip),
          identification(identification) {}

    bool operator==(const TrafficData &other_traffic_data) const {
        return identification == other_traffic_data.identification;
    }

    bool operator<(const TrafficData &other_traffic_data) const {
        return identification < other_traffic_data.identification;
    }

    long long get_time_in_nanoseconds() const { return nanoseconds; }

    int get_type() { return type; }

    Tins::IPv4Address get_source_ip() { return source_ip; }

    Tins::IPv4Address get_destination_ip() { return destination_ip; }

    int get_identification() { return identification; }
};

#endif