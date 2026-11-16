#include "trading/Evidence.hpp"

namespace trading {

TradeRecord createTradeRecord(
    const TradeEvidence& evidence,
    const RRCalculation& rr,
    const DecisionMemo& memo,
    const std::string& orderId,
    OrderType orderType,
    double entry,
    double stop,
    double target,
    MacroRegime regime) {

    TradeRecord record{};
    record.evidence = evidence;
    record.rrCalculation = rr;
    record.memo = memo;
    record.orderId = orderId;
    record.orderType = orderType;
    record.executedEntry = entry;
    record.executedStop = stop;
    record.executedTarget = target;
    record.regimeAtEntry = regime;
    return record;
}

ProvenanceRecord createProvenanceRecord(
    const std::string& tradeId,
    const std::string& timestamp,
    const std::string& signalSource,
    const std::string& macroRead,
    const RRCalculation& rr,
    int positionSize,
    bool wasTaken,
    const std::string& reasonTaken,
    const std::string& reasonExited) {

    ProvenanceRecord record{};
    record.tradeId = tradeId;
    record.timestamp = timestamp;
    record.signalSource = signalSource;
    record.macroRead = macroRead;
    record.grossRR = rr.grossR;
    record.netRR = rr.netR;
    record.positionSize = positionSize;
    record.wasTaken = wasTaken;
    record.reasonTaken = reasonTaken;
    record.reasonExited = reasonExited;
    return record;
}

} // namespace trading