#pragma once

#include "trading/config/AssetSpec.hpp"

#include <chrono>
#include <string>

namespace trading::config {

struct TradingSession {
    bool isTradable{false};
    std::string sessionName;
    std::string reason;
};

[[nodiscard]] TradingSession sessionFilter(
    const AssetSpec& spec,
    std::chrono::system_clock::time_point now);

} // namespace trading::config
