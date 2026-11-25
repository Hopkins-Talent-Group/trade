#include "trading/strategy/Engine.hpp"

namespace trading::strategy {

Engine::Engine(
    const config::AssetSpec& spec,
    ai_filter::IAIClient& ai,
    risk::Manager& risk)
    : spec_(spec), ai_(ai), risk_(risk) {}

double Engine::computeVWAP(double high, double low, double close) const {
    return (high + low + close) / 3.0;
}

double Engine::stopDistance(double price) const {
    // C4: Gold uses Gold-specific volatility; never MNQ point distances for Gold.
    if (spec_.symbol == "MGC" || spec_.symbol == "GC") {
        return price * 0.005;   // 0.5% volatility-based stop for Gold
    }
    return price * 0.003;       // 0.3% for MNQ
}

bool Engine::aiGateAllows(const ai_filter::RegimeOutput& regime, SignalSide side) const {
    // AI gate: veto on "high risk" or bias contradicting the signal.
    if (regime.riskLevel == ai_filter::RiskLevel::High) {
        return false;
    }
    if (side == SignalSide::Long && regime.bias == ai_filter::Bias::Short) {
        return false;
    }
    if (side == SignalSide::Short && regime.bias == ai_filter::Bias::Long) {
        return false;
    }
    return true;
}

std::optional<TradeSignal> Engine::evaluate(const MarketData& data) {
    TradeSignal signal{};

    // C3 risk pre-check (deterministic, no AI).
    const auto lossCheck = risk_.checkDailyLoss(risk_.dailyLossUSD());
    if (!lossCheck.pass) {
        signal.reason = "vetoed: " + lossCheck.reason;
        return std::nullopt;
    }
    const auto countCheck = risk_.checkTradeCount(risk_.tradesTaken());
    if (!countCheck.pass) {
        signal.reason = "vetoed: " + countCheck.reason;
        return std::nullopt;
    }

    // C2 AI gate (data-only).
    const auto regime = ai_filter::evaluateRegime(
        ai_, data.symbol, data.currentPrice, data.vwap, data.recentNewsSummary);

    // Simple VWAP + breakout/retest bias for the scaffold.
    SignalSide side = SignalSide::None;
    if (data.currentPrice > data.vwap) {
        side = SignalSide::Long;
    } else if (data.currentPrice < data.vwap) {
        side = SignalSide::Short;
    }

    if (side == SignalSide::None) {
        signal.reason = "no directional bias vs VWAP";
        return std::nullopt;
    }

    if (!aiGateAllows(regime, side)) {
        signal.reason = "vetoed: AI gate (regime=" + regimeToString(regime.regime)
            + ", bias=" + biasToString(regime.bias)
            + ", risk=" + riskLevelToString(regime.riskLevel) + ")";
        return std::nullopt;
    }

    const double entry = data.currentPrice;
    const double dist = stopDistance(entry);
    double stop = 0.0;
    double target = 0.0;
    if (side == SignalSide::Long) {
        stop = entry - dist;
        target = entry + (2.0 * dist);
    } else {
        stop = entry + dist;
        target = entry - (2.0 * dist);
    }

    // Position sizing via risk manager (exact contracts per asset).
    const auto size = risk_.sizePosition(spec_, 100000.0, 1000.0);
    if (!size.ok) {
        signal.reason = "vetoed: " + size.reason;
        return std::nullopt;
    }

    signal.side = side;
    signal.entry = entry;
    signal.stop = stop;
    signal.target = target;
    signal.contracts = size.contracts;
    signal.reason = "signal approved";
    return signal;
}

} // namespace trading::strategy
