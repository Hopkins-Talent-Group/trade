#pragma once

#include "trading/Types.hpp"

namespace trading {

struct TradeRecord {
    TradeEvidence evidence;
    RRCalculation rrCalculation;
    DecisionMemo memo;
    std::string orderId{};
    OrderType orderType{OrderType::GoodTilCanceled};
    double executedEntry{0.0};
    double executedStop{0.0};
    double executedTarget{0.0};
    MacroRegime regimeAtEntry{MacroRegime::Transitional};
};

struct ProvenanceRecord {
    std::string tradeId{};
    std::string timestamp{};
    std::string signalSource{};
    std::string macroRead{};
    double grossRR{0.0};
    double netRR{0.0};
    int positionSize{0};
    bool wasTaken{false};
    std::string reasonTaken{};
    std::string reasonExited{};
};

} // namespace trading