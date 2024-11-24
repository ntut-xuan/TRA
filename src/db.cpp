#include "db.hpp"
#include "spdlog/spdlog.h"
#include "traffic_data.hpp"

TrafficRecordSingleton* TrafficRecordSingleton::instance = nullptr;
pthread_mutex_t TrafficRecordSingleton::mutex;
std::map<int, TrafficData> TrafficRecordSingleton::traffic_data_with_identification_key;

TrafficRecordSingleton TrafficRecordSingleton::get_instance(){
    if(instance == nullptr){
        instance = new TrafficRecordSingleton();
        pthread_mutex_init(&mutex, 0);
    }
    return *instance;
}

void TrafficRecordSingleton::handle_data(TrafficData data){
    pthread_mutex_lock(&mutex);

    int identification = data.get_identification();
    
    if(traffic_data_with_identification_key.count(identification) > 0){
        traffic_data_with_identification_key.erase(identification);
        spdlog::info("[RESP] Receive response with identification {0} / Waiting Packet {1}", identification, traffic_data_with_identification_key.size());
    }else{
        traffic_data_with_identification_key[identification] = data;
        spdlog::info("[REQ] Receive request with identification {0}", identification);
    }

    pthread_mutex_unlock(&mutex);
}