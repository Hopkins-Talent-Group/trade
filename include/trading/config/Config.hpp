#pragma once

#include "trading/config/AssetSpec.hpp"

#include <string>

namespace trading::config {

struct RiskLimits {
    double maxDailyLossUSD{150.0};
    int maxTradesPerSession{5};
    double softDailyLossAlertUSD{100.0};  // magnet-effect mitigation
};

struct AIFilterConfig {
    std::string provider{"deepseek"};   // "deepseek" or "openai"
    std::string apiKeyEnv{"AI_API_KEY"};
};

struct Config {
    AssetRegistry assets;
    RiskLimits risk;
    AIFilterConfig aiFilter;
};

[[nodiscard]] Config loadConfig(const std::string& yamlPath = "config/config.yaml");

} // namespace trading::config
