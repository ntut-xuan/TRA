#include "spdlog/spdlog.h"
#include "tins/ip.h"
#include <cstdlib>
#include <memory>
#include <pthread.h>
#include <string>
#include <tins/tins.h>
#include <chrono>

#include "protocol.h"
#include "db.hpp"
#include "tins/sniffer.h"
#include "traffic_data.hpp"

#include <date/date.h>

using namespace std::chrono;

typedef std::pair<Tins::Sniffer, std::string> sniffer_bundle;

void* sniffer_worker(void* data){
    sniffer_bundle* sniffer_bundle_data = (sniffer_bundle*)(data);

    sniffer_bundle_data->first.sniff_loop([&](Tins::PDU& pdu) -> bool {
        auto current_time = system_clock::now();
        auto current_time_nanosecond = date::floor<nanoseconds>(current_time);
        Tins::IP packet = pdu.rfind_pdu<Tins::IP>();
        TrafficRecordSingleton::get_instance().handle_data(TrafficData(
            current_time.time_since_epoch().count(),
            packet.protocol(),
            packet.src_addr().to_string().c_str(),
            packet.dst_addr().to_string().c_str(),
            packet.id()
        ));
        return true;
    });

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    if(argc < 3){
        spdlog::error("Error: Should have two port.");
        spdlog::error("Usage: ./TRA <INPUT> <OUTPUT>");
        return 0;
    }

    std::string input_port = argv[1];
    std::string output_port = argv[2];

    spdlog::info("Input Port: {0}", input_port);
    spdlog::info("Output Port: {0}", output_port);
    spdlog::info("Start to capture the packet.");
    
    std::shared_ptr<sniffer_bundle> input_sniffer_bundle = std::make_shared<sniffer_bundle>(input_port, input_port);
    std::shared_ptr<sniffer_bundle> output_sniffer_bundle = std::make_shared<sniffer_bundle>(output_port, output_port);

    pthread_t sniffer_threads[2];
    void** retval = (void**) malloc(sizeof(void*) * 2);
    
    pthread_create(&sniffer_threads[0], NULL, sniffer_worker, (void*) input_sniffer_bundle.get()); 
    pthread_create(&sniffer_threads[1], NULL, sniffer_worker, (void*) output_sniffer_bundle.get()); 

    pthread_join(sniffer_threads[0], &retval[0]);
    pthread_join(sniffer_threads[1], &retval[1]);
}