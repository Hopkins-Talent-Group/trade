#pragma once

#include "trading/Types.hpp"

namespace trading {

[[nodiscard]] MacroRegime classifyRegime(const MarketObservation& observation);

[[nodiscard]] bool isRegimeAlignedWithTrade(
    MacroRegime regime,
    bool isLong);

[[nodiscard]] double adjustSizeForRegime(
    MacroRegime regime,
    double baseSize,
    bool isLong);

[[nodiscard]] double tightenStopForRegime(
    MacroRegime regime,
    double baseStop,
    bool isLong);

} // namespace trading