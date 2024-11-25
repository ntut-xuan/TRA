#ifndef SNIFFER_HPP
#define SNIFFER_HPP

#include "tins/sniffer.h"
#include <memory>
#include <string>

class Sniffer {
private:
    std::shared_ptr<Tins::Sniffer> sniffer;
    std::string device;
    Tins::SnifferConfiguration config;
public:
    Sniffer(std::string device){
        config.set_immediate_mode(true);

        this->device = device;
        this->sniffer = std::make_shared<Tins::Sniffer>(device, config);
    }
    std::string get_device(){
        return device;
    }
    std::shared_ptr<Tins::Sniffer> get_sniffer(){
        return sniffer;
    }
};

#endif