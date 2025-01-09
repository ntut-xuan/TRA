#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>

#include "config.hpp"
#include "spdlog/spdlog.h"

using json = nlohmann::json;

ConfigSingleton *ConfigSingleton::instance = nullptr;
std::string ConfigSingleton::upf_n4_ip = "";
std::string ConfigSingleton::controller_ip = "";
PacketFilterConfiguration ConfigSingleton::input_packet_filter;
PacketFilterConfiguration ConfigSingleton::output_packet_filter;

void ConfigSingleton::load_configuration_file(std::string filepath) {
    std::ifstream config_f(filepath, std::ios::in);
    std::optional<json> config_json_optional;

    if (!config_f.good()) {
        spdlog::error("Configuration load failed since file load failed.");
        return;
    }

    try {
        config_json_optional = std::optional<json>(json::parse(config_f));
    } catch (json::parse_error e) {
        spdlog::error("Configuration load failedsince json parse failed.");
        spdlog::error("Error: {0}", e.what());
        return;
    }

    if (!config_json_optional.has_value()) {
        spdlog::error("Configuration load failed since error occured.");
        return;
    }

    json config_json = config_json_optional.value();

    if (!config_json.contains("self_ip")) {
        spdlog::error("Configurations load failed since self_ip is missing.");
        return;
    }

    std::string self_ip = config_json["self_ip"].get<std::string>();
    spdlog::info("Load configuration - self_ip: {0}", self_ip);

    if (!config_json.contains("controller_ip")) {
        spdlog::error("Configuration load failed since controller_ip is missing");
        return;
    }

    std::string controller_ip = config_json["controller_ip"].get<std::string>();
    spdlog::info("Load configuration - controller_ip: {0}", controller_ip);

    if (!config_json.contains("input_filter")) {
        spdlog::error("Configuration load failed since input_filter is missing");
        return;
    }

    json input_filter = config_json["input_filter"].get<json>();
    std::string input_source_ip = input_filter["source_ip"].get<std::string>();
    std::string input_dest_ip = input_filter["destination_ip"].get<std::string>();

    if (!config_json.contains("output_filter")) {
        spdlog::error("Configuration load failed since input_filter is missing");
        return;
    }

    json output_filter = config_json["output_filter"].get<json>();
    std::string output_source_ip = input_filter["source_ip"].get<std::string>();
    std::string output_dest_ip = input_filter["destination_ip"].get<std::string>();

    ConfigSingleton::setup_input_filter(PacketFilterConfiguration(input_source_ip, input_dest_ip));
    ConfigSingleton::setup_output_filter(PacketFilterConfiguration(output_source_ip, output_dest_ip));
    ConfigSingleton::setup_upf_n4_ip(self_ip);
    ConfigSingleton::setup_controller_ip(controller_ip);

    spdlog::info("Configuration loaded.");
}