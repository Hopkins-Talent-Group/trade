#include "trading/execution/IBKR.hpp"

#include <sstream>

namespace trading::execution {

ExecutionResult StubGateway::submitBracket(const strategy::TradeSignal& signal) {
    ExecutionResult result{};
    if (signal.side == strategy::SignalSide::None || signal.contracts < 1) {
        result.accepted = false;
        result.reason = "no valid signal to execute";
        return result;
    }

    BracketOrder bracket{};
    bracket.orderId = "ORD-" + std::to_string(nextId_++);
    bracket.entryOrderId = bracket.orderId + "-ENTRY";
    bracket.stopOrderId = bracket.orderId + "-STOP";
    bracket.targetOrderId = bracket.orderId + "-TARGET";
    bracket.entryPrice = signal.entry;
    bracket.stopPrice = signal.stop;
    bracket.targetPrice = signal.target;
    bracket.contracts = signal.contracts;
    bracket.isAutomated = true;
    bracket.stopAttached = true;  // stub: assume attach succeeds

    result.accepted = true;
    result.bracket = bracket;
    result.reason = "bracket order submitted";
    return result;
}

bool StubGateway::attachStopLoss(const std::string& orderId, double stopPrice) {
    (void)orderId;
    (void)stopPrice;
    return true;  // stub: stop attach succeeds
}

bool StubGateway::closePosition(const std::string& orderId) {
    (void)orderId;
    return true;  // stub: close succeeds
}

void StubGateway::disconnect() {
    // stub: no-op
}

} // namespace trading::execution
