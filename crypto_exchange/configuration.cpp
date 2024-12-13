#include <fstream>
#include <nlohmann/json.hpp>
#include "crypto.h"
using namespace std;
using namespace nlohmann;

Configuration::Configuration (string const& configFileName) {
    fstream configFile (configFileName);
    
    auto jConfig = json::parse (configFile);
    auto jLots = jConfig["lots"];

    ip = jConfig["database_ip"].get<string>();
    port = jConfig["database_port"].get<size_t>();
    lots = {jLots.begin(), jLots.end()};
}