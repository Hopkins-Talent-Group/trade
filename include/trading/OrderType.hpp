#pragma once

#include "trading/Types.hpp"

namespace trading {

[[nodiscard]] OrderType orderTypeForHorizon(
    const std::string& thesisHorizon);

[[nodiscard]] std::string horizonForOrderType(OrderType orderType);

[[nodiscard]] bool isOrderGTC(OrderType orderType);

} // namespace trading