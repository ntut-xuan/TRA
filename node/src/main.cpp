#include <chrono>
#include <cstdlib>
#include <ctime>
#include <date/date.h>
#include <memory>
#include <pthread.h>
#include <sched.h>
#include <string>
#include <tins/tins.h>
#include <unistd.h>

#include "config.hpp"
#include "ip_util.hpp"
#include "postman.hpp"
#include "sniffer.hpp"
#include "spdlog/spdlog.h"
#include "tins/ip.h"
#include "tins/sniffer.h"

#include "cpu.hpp"
#include "db.hpp"
#include "traffic_data.hpp"
#include "traffic_stat.hpp"

using namespace std::chrono;

typedef std::pair<Tins::Sniffer, std::string> sniffer_bundle;

void *sniffer_worker(void *data) {
    Sniffer *sniffer = (Sniffer *)(data);

    sniffer->get_sniffer()->sniff_loop([&](Tins::PDU &pdu) -> bool {
        auto current_time = system_clock::now();
        auto current_time_nanosecond = date::floor<nanoseconds>(current_time);
        Tins::IP packet = pdu.rfind_pdu<Tins::IP>();
        TrafficRecordSingleton::get_instance().handle_data(
            TrafficData(current_time.time_since_epoch().count(), packet.protocol(),
                        packet.src_addr().to_string().c_str(), packet.dst_addr().to_string().c_str(), packet.id()));
        return true;
    });

    pthread_exit(NULL);
}

void *print_report(void *data) {
    while (true) {
        int current_timestamp = time(0);
        int query_timestamp = current_timestamp - 1;

        TrafficStat traffic_stat = TrafficRecordSingleton::get_instance().get_traffic_stat(query_timestamp);

        spdlog::info(
            "[REPORT] Timestamp {1} / Packet Loss {0}% / RECV {2} / TRANSMIT {3} / QUEUEING_DALAY {4} ms / CPU {5}%",
            traffic_stat.get_packet_loss() * 100, query_timestamp, traffic_stat.get_received_packet(),
            traffic_stat.get_transmitted_packet(), traffic_stat.get_queueing_delay_in_nanosecond() / 1e6,
            traffic_stat.get_cpu_usage());

        submit_report(traffic_stat, ConfigSingleton::get_controller_ip(), 8805);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        spdlog::error("Error: Should have two port.");
        spdlog::error("Usage: ./TRA <INPUT> <OUTPUT> <UPFN4IP> | <CONTROLLER_IP> ");
        return 0;
    }

    if (argc < 4) {
        spdlog::error("Error: Should have UPF N4 IP.");
        spdlog::error("Usage: ./TRA <INPUT> <OUTPUT> <UPFN4IP> | <CONTROLLER_IP> ");
        return 0;
    }

    std::string input_port = argv[1];
    std::string output_port = argv[2];
    std::string upfn4ip = argv[3];

    ConfigSingleton::get_instance().setup_upf_n4_ip(upfn4ip);

    if (argc == 5) {
        std::string controller_ip = argv[4];
        ConfigSingleton::get_instance().setup_controller_ip(controller_ip);
    }

    spdlog::info("Input Port: {0}", input_port);
    spdlog::info("Output Port: {0}", output_port);
    spdlog::info("IP: {0}({1})", upfn4ip, convert_ip_str_to_uint32_t(upfn4ip));

    if (ConfigSingleton::get_instance().is_setup_controller_ip()) {
        spdlog::info("Controller IP: {0}", ConfigSingleton::get_controller_ip());
    }

    spdlog::info("Start to capture the packet.");

    TrafficRecordSingleton::get_instance().setup_upfn4ip(convert_ip_str_to_uint32_t(upfn4ip));

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