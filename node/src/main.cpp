#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <date/date.h>
#include <memory>
#include <pthread.h>
#include <sched.h>
#include <string>
#include <tins/tins.h>
#include <unistd.h>

#include "argh.h"
#include "cpu.hpp"
#include "gtpu_identifier.hpp"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tins/ip.h"
#include "tins/sniffer.h"

#include "config.hpp"
#include "db.hpp"
#include "ip_util.hpp"
#include "postman.hpp"
#include "sniffer.hpp"
#include "tins/udp.h"
#include "traffic_data.hpp"
#include "traffic_stat.hpp"

using namespace std::chrono;

void *sniffer_worker(void *data) {
    Sniffer *sniffer = (Sniffer *)(data);

    sniffer->get_sniffer()->sniff_loop([&](Tins::PDU &pdu) -> bool {
        auto current_time = system_clock::now();
        auto current_time_nanosecond = date::floor<nanoseconds>(current_time);
        Tins::IP packet = pdu.rfind_pdu<Tins::IP>();
        int identification = packet.id();

        Tins::UDP *udp = pdu.find_pdu<Tins::UDP>();
        if (udp != nullptr && is_gtpu_packet(udp)) {
            spdlog::debug("Found GTP-U Packet / UDP CHKSUM {0}", udp->checksum());
            identification = extract_gtpu_packet_identification(udp);
        }

        TrafficRecordSingleton::get_instance().handle_data(
            TrafficData(current_time.time_since_epoch().count(), packet.protocol(),
                        packet.src_addr().to_string().c_str(), packet.dst_addr().to_string().c_str(), identification));
        return true;
    });

    pthread_exit(NULL);
}

void *print_report(void *data) {
    while (true) {
        int current_timestamp = time(0);
        int query_timestamp = current_timestamp - 1;

        TrafficStat traffic_stat = TrafficRecordSingleton::get_instance().get_traffic_stat(query_timestamp);
        double cpu_usage = fetch_cpu_usage();

        spdlog::info(
            "[REPORT] Timestamp {1} / Packet Loss {0}% / RECV {2} / TRANSMIT {3} / QUEUEING_DALAY {4} ms / CPU {5}%",
            traffic_stat.get_packet_loss() * 100, query_timestamp, traffic_stat.get_received_packet(),
            traffic_stat.get_transmitted_packet(), traffic_stat.get_queueing_delay_in_nanosecond() / 1e6, cpu_usage);

        create_spacp(traffic_stat, query_timestamp);
        submit_report(traffic_stat, ConfigSingleton::get_controller_ip(), 8805, cpu_usage);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    argh::parser parser;
    parser.add_params({"-i", "--input", "-o", "--output", "--config-file"});
    parser.parse(argc, argv);

    std::string input_port;
    if (!(parser({"-i", "--input"}) >> input_port)) {
        spdlog::error("Error: Need input port, got '{0}'.", parser("input").str());
        spdlog::error("Usage: ./TRA -i <INPUT> -o <OUTPUT> --config-file <CONFIG_FILE_PATH> ");
        return -1;
    }

    std::string output_port;
    if (!(parser({"-o", "--output"}) >> output_port)) {
        spdlog::error("Error: Need output port.");
        spdlog::error("Usage: ./TRA -i <INPUT> -o <OUTPUT> --config-file <CONFIG_FILE_PATH> ");
        return -1;
    }

    std::string config_path;
    if ((parser({"--config-file"}) >> config_path)) {
        ConfigSingleton::load_configuration_file(config_path);
    }

    spdlog::info("Input Port: {0}", input_port);
    spdlog::info("Output Port: {0}", output_port);
    spdlog::info("IP: {0}({1})", ConfigSingleton::get_upf_n4_ip(),
                 convert_ip_str_to_uint32_t(ConfigSingleton::get_upf_n4_ip()));
    spdlog::info("Input Filter: Source IP {0} / Destination IP {1}",
                 ConfigSingleton::get_input_filter().get_source_ip(),
                 ConfigSingleton::get_input_filter().get_destination_ip());
    spdlog::info("Output Filter: Source IP {0} / Destination IP {1}",
                 ConfigSingleton::get_output_filter().get_source_ip(),
                 ConfigSingleton::get_output_filter().get_destination_ip());

    if (ConfigSingleton::get_instance().is_setup_controller_ip()) {
        spdlog::info("Controller IP: {0}", ConfigSingleton::get_controller_ip());
    }

    spdlog::info("Start to capture the packet.");

    TrafficRecordSingleton::get_instance().setup_upfn4ip(convert_ip_str_to_uint32_t(ConfigSingleton::get_upf_n4_ip()));

    Tins::SnifferConfiguration config;
    config.set_immediate_mode(true);

    std::shared_ptr<Sniffer> input_sniffer = std::make_shared<Sniffer>(input_port);
    std::shared_ptr<Sniffer> output_sniffer = std::make_shared<Sniffer>(output_port);

    pthread_t sniffer_threads[2];
    pthread_t report_threads;

    pthread_create(&sniffer_threads[0], NULL, sniffer_worker, (void *)input_sniffer.get());
    pthread_create(&sniffer_threads[1], NULL, sniffer_worker, (void *)output_sniffer.get());
    pthread_create(&report_threads, NULL, print_report, nullptr);

    struct sched_param sniffer_sched_param[2];
    sniffer_sched_param[0].sched_priority = 80;
    sniffer_sched_param[1].sched_priority = 80;

    struct sched_param report_sched_param;
    report_sched_param.sched_priority = 10;

    pthread_setschedparam(sniffer_threads[0], SCHED_RR, &sniffer_sched_param[0]);
    pthread_setschedparam(sniffer_threads[1], SCHED_RR, &sniffer_sched_param[1]);
    pthread_setschedparam(report_threads, SCHED_RR, &report_sched_param);

    spdlog::info("Thread join");

    pthread_join(sniffer_threads[0], NULL);
    pthread_join(sniffer_threads[1], NULL);
}