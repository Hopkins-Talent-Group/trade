#include "trading/risk/Manager.hpp"

namespace trading::risk {

Manager::Manager(const RiskLimits& limits) : limits_(limits) {}

RiskCheck Manager::checkDailyLoss() const {
    RiskCheck check{};
    if (killSwitchArmed_) {
        check.pass = false;
        check.reason = "kill switch armed";
        return check;
    }
    if (dailyLossUSD_ <= -limits_.maxDailyLossUSD) {
        check.pass = false;
        check.reason = "max daily loss reached";
        return check;
    }
    check.pass = true;
    check.reason = "daily loss within limit";
    return check;
}

RiskCheck Manager::checkProjectedLoss(double projectedPnLUSD) const {
    RiskCheck check{};
    if (killSwitchArmed_) {
        check.pass = false;
        check.reason = "kill switch armed";
        return check;
    }
    if (dailyLossUSD_ + projectedPnLUSD <= -limits_.maxDailyLossUSD) {
        check.pass = false;
        check.reason = "projected loss would exceed daily limit";
        return check;
    }
    check.pass = true;
    check.reason = "projected loss within limit";
    return check;
}

RiskCheck Manager::checkTradeCount() const {
    RiskCheck check{};
    if (killSwitchArmed_) {
        check.pass = false;
        check.reason = "kill switch armed";
        return check;
    }
    if (tradesTaken_ >= limits_.maxTradesPerSession) {
        check.pass = false;
        check.reason = "max trades per session reached";
        return check;
    }
    check.pass = true;
    check.reason = "trade count within limit";
    return check;
}

PositionSizeResult Manager::sizePosition(
    const config::AssetSpec& spec,
    double accountEquityUSD,
    double riskPerTradeUSD) const {

    PositionSizeResult result{};
    if (spec.tickSize <= 0.0 || spec.tickValue <= 0.0 || spec.marginLimit <= 0.0) {
        result.ok = false;
        result.reason = "invalid asset spec";
        return result;
    }
    if (killSwitchArmed_) {
        result.ok = false;
        result.reason = "kill switch armed";
        return result;
    }

    // Risk per trade in USD divided by per-tick value gives max ticks risked;
    // convert to contracts keeping within margin limit.
    const double riskUSD = (riskPerTradeUSD > 0.0) ? riskPerTradeUSD
                                                   : (accountEquityUSD * 0.01);
    const int contractsByRisk = static_cast<int>(riskUSD / spec.tickValue);
    const int contractsByMargin = static_cast<int>(accountEquityUSD / spec.marginLimit);

    int contracts = 0;
    if (contractsByRisk > 0 && contractsByMargin > 0) {
        contracts = (contractsByRisk < contractsByMargin) ? contractsByRisk : contractsByMargin;
    }
    if (contracts < 1) {
        contracts = 1;  // minimum 1 contract per directive (1 MNQ vs 1 MGC)
    }

    result.contracts = contracts;
    result.notionalUSD = contracts * spec.marginLimit;
    result.ok = true;
    result.reason = "position sized";
    return result;
}

void Manager::recordTrade(double pnlUSD) {
    dailyLossUSD_ += pnlUSD;
    ++tradesTaken_;
    if (dailyLossUSD_ <= -limits_.maxDailyLossUSD) {
        armKillSwitch();
    }
}

void Manager::resetSession() {
    dailyLossUSD_ = 0.0;
    tradesTaken_ = 0;
    disarmKillSwitch();
}

bool Manager::killSwitchArmed() const { return killSwitchArmed_; }
void Manager::armKillSwitch() { killSwitchArmed_ = true; }
void Manager::disarmKillSwitch() { killSwitchArmed_ = false; }

double Manager::dailyLossUSD() const { return dailyLossUSD_; }
int Manager::tradesTaken() const { return tradesTaken_; }

} // namespace trading::risk
