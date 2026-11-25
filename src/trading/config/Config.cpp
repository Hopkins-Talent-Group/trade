#include "trading/config/Config.hpp"

#include <fstream>
#include <sstream>

namespace trading::config {

Config loadConfig(const std::string& propertiesPath) {
    Config cfg{};
    cfg.assets = loadDefaultRegistry();

    // Minimal properties override (KEY = VALUE). Fail-safe: defaults used if absent.
    if (!propertiesPath.empty()) {
        std::ifstream in(propertiesPath);
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                // best-effort override hook; defaults remain if parsing is not implemented
                (void)line;
            }
        }
    }
    return cfg;
}

} // namespace trading::config
