#include <algorithm>

#include "db.hpp"
#include "traffic_data.hpp"
#include "traffic_stat.hpp"

TrafficRecordSingleton *TrafficRecordSingleton::instance = nullptr;
pthread_mutex_t TrafficRecordSingleton::mutex;
std::map<int, TrafficData> TrafficRecordSingleton::traffic_data_with_identification_key;
std::map<int, TrafficStat> TrafficRecordSingleton::traffic_stat_with_timestamp_key;
int TrafficRecordSingleton::newset_timestamp = 0;
uint32_t TrafficRecordSingleton::upfn4ip = 0;

TrafficRecordSingleton TrafficRecordSingleton::get_instance() {
    if (instance == nullptr) {
        instance = new TrafficRecordSingleton();
        pthread_mutex_init(&mutex, 0);
    }
    return *instance;
}

void TrafficRecordSingleton::setup_upfn4ip(uint32_t ip) { this->upfn4ip = ip; }

void TrafficRecordSingleton::handle_data(TrafficData data) {
    pthread_mutex_lock(&mutex);

    int identification = data.get_identification();
    int timestamp = data.get_time_in_nanoseconds() / 1e9;

    newset_timestamp = std::max(timestamp, newset_timestamp);

    if (traffic_stat_with_timestamp_key.count(timestamp) == 0) {
        traffic_stat_with_timestamp_key[timestamp] = TrafficStat(upfn4ip, timestamp);
    }

    traffic_stat_with_timestamp_key[timestamp].handle_packet(data);

    pthread_mutex_unlock(&mutex);
}

std::pair<int, TrafficStat> TrafficRecordSingleton::get_newest_traffic_stat() {
    pthread_mutex_lock(&mutex);

    TrafficStat traffic_stat = traffic_stat_with_timestamp_key[newset_timestamp - 1];

    pthread_mutex_unlock(&mutex);

    return std::pair<int, TrafficStat>(newset_timestamp - 1, traffic_stat);
}

TrafficStat TrafficRecordSingleton::get_traffic_stat(int timestamp) {
    pthread_mutex_lock(&mutex);

    TrafficStat traffic_stat = traffic_stat_with_timestamp_key[timestamp];

    pthread_mutex_unlock(&mutex);

    return traffic_stat;
}