#pragma once

#include "trading/strategy/Engine.hpp"

#include <optional>
#include <string>

namespace trading::execution {

struct BracketOrder {
    std::string orderId;
    std::string entryOrderId;
    std::string stopOrderId;
    std::string targetOrderId;
    double entryPrice{0.0};
    double stopPrice{0.0};
    double targetPrice{0.0};
    int contracts{0};
    bool isAutomated{true};
    bool stopAttached{false};
};

struct ExecutionResult {
    bool accepted{false};
    BracketOrder bracket{};
    std::string reason;
    bool positionClosed{false};
};

class IGateway {
public:
    virtual ~IGateway() = default;

    [[nodiscard]] virtual ExecutionResult submitBracket(const strategy::TradeSignal& signal) = 0;
    [[nodiscard]] virtual bool attachStopLoss(const std::string& orderId, double stopPrice) = 0;
    [[nodiscard]] virtual bool closePosition(const std::string& orderId) = 0;
    virtual void disconnect() = 0;
};

class StubGateway : public IGateway {
public:
    [[nodiscard]] ExecutionResult submitBracket(const strategy::TradeSignal& signal) override;
    [[nodiscard]] bool attachStopLoss(const std::string& orderId, double stopPrice) override;
    [[nodiscard]] bool closePosition(const std::string& orderId) override;
    void disconnect() override;

private:
    int nextId_{1};
};

} // namespace trading::execution
