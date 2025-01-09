#ifndef CONFIG_SINGLETON_HPP
#define CONFIG_SINGLETON_HPP

#include <string>

class PacketFilterConfiguration {
  private:
    std::string source_ip = "";
    std::string destination_ip = "";

  public:
    PacketFilterConfiguration() = default;
    PacketFilterConfiguration(std::string source_ip, std::string destination_ip) {
        this->source_ip = source_ip;
        this->destination_ip = destination_ip;
    }
    std::string get_source_ip() { return source_ip; }
    std::string get_destination_ip() { return destination_ip; }
    void set_source_ip(std::string source_ip) { this->source_ip = source_ip; }
    void set_destination_ip(std::string destinaltion_ip) { this->destination_ip = destinaltion_ip; }
};

class ConfigSingleton {
  private:
    static ConfigSingleton *instance;
    static std::string upf_n4_ip;
    static std::string controller_ip;
    static PacketFilterConfiguration input_packet_filter;
    static PacketFilterConfiguration output_packet_filter;

  public:
    static ConfigSingleton get_instance() {
        if (instance == nullptr) {
            instance = new ConfigSingleton();
        }
        return *instance;
    }
    static void setup_upf_n4_ip(std::string upfn4ip) { upf_n4_ip = upfn4ip; }
    static void setup_controller_ip(std::string ctrlip) { controller_ip = ctrlip; }
    static bool is_setup_controller_ip() { return controller_ip.length() > 0; }
    static std::string get_controller_ip() { return controller_ip; }
    static std::string get_upf_n4_ip() { return upf_n4_ip; }
    static void load_configuration_file(std::string filepath);
    static void setup_input_filter(PacketFilterConfiguration configuration) { input_packet_filter = configuration; }
    static void setup_output_filter(PacketFilterConfiguration configuration) { output_packet_filter = configuration; };
    static PacketFilterConfiguration get_input_filter() { return input_packet_filter; }
    static PacketFilterConfiguration get_output_filter() { return output_packet_filter; }
};

#endif