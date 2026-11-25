#pragma once

#include "trading/config/AssetSpec.hpp"

#include <string>

namespace trading::config {

struct Config {
    AssetRegistry assets;
};

[[nodiscard]] Config loadConfig(const std::string& propertiesPath = "");

} // namespace trading::config
