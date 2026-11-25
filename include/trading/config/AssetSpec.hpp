#pragma once

#include <string>

namespace trading::config {

struct AssetSpec {
    std::string symbol;
    double tickSize{0.0};
    double tickValue{0.0};
    double marginLimit{0.0};
    std::string session;
};

struct AssetRegistry {
    AssetSpec mnq;
    AssetSpec mgc;
    AssetSpec gc;
};

[[nodiscard]] AssetRegistry loadDefaultRegistry();
[[nodiscard]] const AssetSpec& specFor(const AssetRegistry& registry, const std::string& symbol);

} // namespace trading::config
