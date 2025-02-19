#ifndef PFCP_HPP
#define PFCP_HPP

#include "traffic_stat.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

static int extract_float(double x) {
    int x_int = std::floor(x);
    return (x - x_int) * 1e6;
}

static void concatenate_uint8t_array(std::vector<uint8_t> &array, std::vector<uint8_t> other) {
    array.insert(array.end(), other.begin(), other.end());
}

static std::vector<uint8_t> to_uint8_t_arrays(uint64_t val, int length) {
    std::vector<uint8_t> datas(length);
    int index = length - 1;
    while (val > 0) {
        datas[index--] = val % 256;
        val /= 256;
    }
    return datas;
}

inline std::vector<uint8_t> convert_traffic_stat_into_pfcp_packet(TrafficStat stat, double cpu_usage) {
    std::vector<uint8_t> packet;

    /* --- PFCP Header --- */

    concatenate_uint8t_array(packet, to_uint8_t_arrays(0x20, 1));   // Flag
    concatenate_uint8t_array(packet, to_uint8_t_arrays(22, 1));     // Type
    concatenate_uint8t_array(packet, to_uint8_t_arrays(4 + 27, 2)); // Length
    concatenate_uint8t_array(packet, to_uint8_t_arrays(0, 3));      // Sequence
    concatenate_uint8t_array(packet, to_uint8_t_arrays(0, 1));      // Spare

    /* ------------------- */

    /* --- IE Header --- */

    std::vector<uint8_t> upfn4ip_vec = to_uint8_t_arrays(stat.get_upf_n4_ip(), 4);
    std::reverse(upfn4ip_vec.begin(), upfn4ip_vec.end());

    concatenate_uint8t_array(packet, to_uint8_t_arrays(21122, 2));     // IE Type
    concatenate_uint8t_array(packet, to_uint8_t_arrays(25, 2));        // IE Length
    concatenate_uint8t_array(packet, to_uint8_t_arrays(0, 1));         // Spare
    concatenate_uint8t_array(packet, upfn4ip_vec);                     // UPFN4IP
    concatenate_uint8t_array(packet, to_uint8_t_arrays(cpu_usage, 2)); // CPU Usage

    /* Packet Loss */
    double packet_loss = stat.get_packet_loss();
    int packet_loss_int = std::round(stat.get_packet_loss());
    int packet_loss_float = extract_float(packet_loss);
    concatenate_uint8t_array(packet, to_uint8_t_arrays(packet_loss_int, 4));   // PACKET LOSS INT
    concatenate_uint8t_array(packet, to_uint8_t_arrays(packet_loss_float, 4)); // PACKET LOST FLOAT

    /* Queueing Delay */
    double queueing_delay_millisecond = stat.get_queueing_delay_in_nanosecond() / 1e6;
    int queueing_delay_millisecond_int = std::round(queueing_delay_millisecond);
    int queueing_delay_millisecond_float = extract_float(queueing_delay_millisecond);
    concatenate_uint8t_array(packet, to_uint8_t_arrays(queueing_delay_millisecond_int, 4));   // QUEUEING DELAY INT
    concatenate_uint8t_array(packet, to_uint8_t_arrays(queueing_delay_millisecond_float, 4)); // QUEUEING DELAY FLOAT

    /* ---------------- */

    return packet;
}

#endif