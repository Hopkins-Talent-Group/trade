#include "trading/config/AssetSpec.hpp"

namespace trading::config {

AssetRegistry loadDefaultRegistry() {
    AssetRegistry registry{};
    registry.mnq = {"MNQ", 0.25, 0.50, 500.0, "us_regular"};
    registry.mgc = {"MGC", 0.10, 1.00, 1000.0, "twenty_four_hour"};
    registry.gc = {"GC", 0.10, 10.00, 5000.0, "twenty_four_hour"};
    return registry;
}

const AssetSpec& specFor(const AssetRegistry& registry, const std::string& symbol) {
    if (symbol == "MGC") {
        return registry.mgc;
    }
    if (symbol == "GC") {
        return registry.gc;
    }
    return registry.mnq;
}

} // namespace trading::config
