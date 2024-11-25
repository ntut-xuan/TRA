#include "db.hpp"
#include "spdlog/spdlog.h"
#include "traffic_data.hpp"
#include "traffic_stat.hpp"
#include <algorithm>

TrafficRecordSingleton *TrafficRecordSingleton::instance = nullptr;
pthread_mutex_t TrafficRecordSingleton::mutex;
std::map<int, TrafficData> TrafficRecordSingleton::traffic_data_with_identification_key;
std::map<int, TrafficStat> TrafficRecordSingleton::traffic_stat_with_timestamp_key;
int TrafficRecordSingleton::newset_timestamp = 0;

TrafficRecordSingleton TrafficRecordSingleton::get_instance() {
    if (instance == nullptr) {
        instance = new TrafficRecordSingleton();
        pthread_mutex_init(&mutex, 0);
    }
    return *instance;
}

void TrafficRecordSingleton::handle_data(TrafficData data) {
    pthread_mutex_lock(&mutex);

    int identification = data.get_identification();
    int timestamp = data.get_time_in_nanoseconds() / 1e9;
    
    newset_timestamp = std::max(timestamp, newset_timestamp);

    if (traffic_stat_with_timestamp_key.count(timestamp) == 0){
        traffic_stat_with_timestamp_key[timestamp] = TrafficStat();
    }

    traffic_stat_with_timestamp_key[timestamp].handle_packet(data);

    pthread_mutex_unlock(&mutex);
}

std::pair<int, TrafficStat> TrafficRecordSingleton::get_newest_traffic_stat(){
    pthread_mutex_lock(&mutex);

    TrafficStat traffic_stat = traffic_stat_with_timestamp_key[newset_timestamp-1];

    pthread_mutex_unlock(&mutex);

    return std::pair<int, TrafficStat>(newset_timestamp-1, traffic_stat);
}