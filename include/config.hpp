#ifndef CONFIG_SINGLETON_HPP
#define CONFIG_SINGLETON_HPP

#include <string>

class ConfigSingleton {
  private:
    static ConfigSingleton *instance;
    static std::string upf_n4_ip;
    static std::string controller_ip;

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
};

#endif