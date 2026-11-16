#include "trading/EntryValidator.hpp"

namespace trading {

ValidationResult validateEntryFirst(
    double entry,
    double stop,
    const std::string& invalidationCondition) {

    ValidationResult result;
    result.valid = false;

    if (std::abs(stop - entry) < 1e-12) {
        result.error = "Stop price cannot be equal to entry price.";
        return result;
    }

    result.point = makeInvalidationPoint(stop, invalidationCondition, false);
    result.valid = true;
    return result;
}

InvalidationPoint makeInvalidationPoint(
    double price,
    const std::string& condition,
    bool isMacroChange) {

    InvalidationPoint point{};
    point.price = price;
    point.condition = condition;
    point.isMacroChange = isMacroChange;
    return point;
}

} // namespace trading