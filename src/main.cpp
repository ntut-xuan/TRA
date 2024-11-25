#include "sniffer.hpp"
#include "spdlog/spdlog.h"
#include "tins/ip.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <pthread.h>
#include <sched.h>
#include <string>
#include <tins/tins.h>

#include "db.hpp"
#include "tins/sniffer.h"
#include "traffic_data.hpp"
#include "traffic_stat.hpp"

#include <date/date.h>
#include <unistd.h>
#include "cpu.hpp"

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
    while(true){
        int current_timestamp = time(0);

        std::pair<int, TrafficStat> traffic_stat_pair = TrafficRecordSingleton::get_instance().get_newest_traffic_stat();
        int query_timestamp = traffic_stat_pair.first;
        TrafficStat traffic_stat = traffic_stat_pair.second;

        if(query_timestamp == 0){
            continue;
        }
        
        spdlog::info("[REPORT] Timestamp {1} / Packet Loss {0}% / RECV {2} / TRANSMIT {3} / QUEUEING_DALAY {4} ms / CPU {5}%",
                    traffic_stat.get_packet_loss() * 100, query_timestamp, traffic_stat.get_received_packet(),
                    traffic_stat.get_transmitted_packet(), traffic_stat.get_queueing_delay_in_nanosecond() / 1e6, fetch_cpu_usage());

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        spdlog::error("Error: Should have two port.");
        spdlog::error("Usage: ./TRA <INPUT> <OUTPUT>");
        return 0;
    }

    std::string input_port = argv[1];
    std::string output_port = argv[2];

    spdlog::info("Input Port: {0}", input_port);
    spdlog::info("Output Port: {0}", output_port);
    spdlog::info("Start to capture the packet.");

    Tins::SnifferConfiguration config;
    config.set_immediate_mode(true);


    std::shared_ptr<Sniffer> input_sniffer = std::make_shared<Sniffer>(input_port);
    std::shared_ptr<Sniffer> output_sniffer = std::make_shared<Sniffer>(output_port);

    pthread_t sniffer_threads[2];
    pthread_t report_threads;
    void **retval = (void **)malloc(sizeof(void *) * 3);

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

    pthread_join(sniffer_threads[0], &retval[0]);
    pthread_join(sniffer_threads[1], &retval[1]);
    pthread_join(report_threads, &retval[2]);
}