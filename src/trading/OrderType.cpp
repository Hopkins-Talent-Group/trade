#include "trading/OrderType.hpp"

#include <unordered_map>

namespace trading {

namespace {

const std::unordered_map<std::string, OrderType> horizonToOrderType{
    {"intraday", OrderType::DayOrder},
    {"multi-day", OrderType::GoodTilCanceled},
    {"conditional", OrderType::StopLimit},
    {"deep-value", OrderType::AtTargetPrice}
};

const std::unordered_map<OrderType, std::string> orderTypeToHorizon{
    {OrderType::DayOrder, "intraday"},
    {OrderType::GoodTilCanceled, "multi-day"},
    {OrderType::StopLimit, "conditional"},
    {OrderType::AtTargetPrice, "deep-value"}
};

} // namespace

OrderType orderTypeForHorizon(const std::string& thesisHorizon) {
    auto it = horizonToOrderType.find(thesisHorizon);
    if (it != horizonToOrderType.end()) {
        return it->second;
    }
    return OrderType::GoodTilCanceled;
}

std::string horizonForOrderType(OrderType orderType) {
    auto it = orderTypeToHorizon.find(orderType);
    if (it != orderTypeToHorizon.end()) {
        return it->second;
    }
    return "multi-day";
}

bool isOrderGTC(OrderType orderType) {
    return orderType == OrderType::GoodTilCanceled ||
           orderType == OrderType::AtTargetPrice;
}

} // namespace trading