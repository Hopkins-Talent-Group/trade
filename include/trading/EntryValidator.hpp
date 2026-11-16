#pragma once

#include "trading/Types.hpp"

namespace trading {

struct ValidationResult {
    bool valid{false};
    InvalidationPoint point{};
    std::string error{};
};

[[nodiscard]] ValidationResult validateEntryFirst(
    double entry,
    double stop,
    const std::string& invalidationCondition);

[[nodiscard]] InvalidationPoint makeInvalidationPoint(
    double price,
    const std::string& condition,
    bool isMacroChange);

} // namespace trading