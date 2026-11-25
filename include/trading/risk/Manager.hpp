#pragma once

#include "trading/config/AssetSpec.hpp"

#include <string>

namespace trading::risk {

struct RiskLimits {
    double maxDailyLossUSD{150.0};
    int maxTradesPerSession{5};
};

struct RiskCheck {
    bool pass{false};
    std::string reason;
};

struct PositionSizeResult {
    int contracts{0};
    double notionalUSD{0.0};
    bool ok{false};
    std::string reason;
};

class Manager {
public:
    explicit Manager(const RiskLimits& limits);

    [[nodiscard]] RiskCheck checkDailyLoss(double currentPnLUSD) const;
    [[nodiscard]] RiskCheck checkTradeCount(int tradesTaken) const;

    [[nodiscard]] PositionSizeResult sizePosition(
        const config::AssetSpec& spec,
        double accountEquityUSD,
        double riskPerTradeUSD) const;

    void recordTrade(double pnlUSD);
    void resetSession();

    [[nodiscard]] bool killSwitchArmed() const;
    void armKillSwitch();
    void disarmKillSwitch();

    [[nodiscard]] double dailyLossUSD() const;
    [[nodiscard]] int tradesTaken() const;

private:
    RiskLimits limits_;
    double dailyLossUSD_{0.0};
    int tradesTaken_{0};
    bool killSwitchArmed_{false};
};

} // namespace trading::risk
