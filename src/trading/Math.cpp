#include "trading/Math.hpp"

#include <stdexcept>

namespace trading {

RRCalculation calculateFeeAdjustedRR(
    double entry,
    double stop,
    double target,
    const FeeAdjustment& costs) {

    RRCalculation result{};
    result.entry = entry;
    result.stop = stop;
    result.target = target;
    result.costs = costs;

    const double grossRR = (target - entry) / (entry - stop);
    result.grossR = grossRR;

    const double netNumerator = (target - entry - costs.total);
    const double netDenominator = (entry - stop + costs.total);

    if (std::abs(netDenominator) > 1e-12) {
        result.netR = netNumerator / netDenominator;
    } else {
        result.netR = 0.0;
    }

    return result;
}

bool isTradeWorthTaking(const RRCalculation& calc, double minNetRR) {
    return calc.netR >= minNetRR;
}

} // namespace trading