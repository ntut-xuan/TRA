#ifndef TRAFFIC_RECORD_SINGLETON_HPP
#define TRAFFIC_RECORD_SINGLETON_HPP

#include <map>
#include <pthread.h>

#include "traffic_data.hpp"
#include "traffic_stat.hpp"

class TrafficRecordSingleton {
  private:
    static TrafficRecordSingleton *instance;
    static pthread_mutex_t mutex;
    static std::map<int, TrafficData> traffic_data_with_identification_key;
    static std::map<int, TrafficStat> traffic_stat_with_timestamp_key;
    static int newset_timestamp;
    TrafficRecordSingleton() {}

  public:
    static TrafficRecordSingleton get_instance();
    void handle_data(TrafficData data);
    std::pair<int, TrafficStat> get_newest_traffic_stat();
};

#endif