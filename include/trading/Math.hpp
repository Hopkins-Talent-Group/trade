#pragma once

#include "trading/Types.hpp"
#include <cmath>

namespace trading {

[[nodiscard]] RRCalculation calculateFeeAdjustedRR(
    double entry,
    double stop,
    double target,
    const FeeAdjustment& costs);

[[nodiscard]] bool isTradeWorthTaking(const RRCalculation& calc, double minNetRR = 1.5);

} // namespace trading