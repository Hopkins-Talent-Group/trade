#pragma once

#include "trading/ai_filter/Regime.hpp"
#include "trading/config/AssetSpec.hpp"
#include "trading/risk/Manager.hpp"

#include <optional>
#include <string>

namespace trading::strategy {

enum class SignalSide { Long, Short, None };

struct TradeSignal {
    SignalSide side{SignalSide::None};
    double entry{0.0};
    double stop{0.0};
    double target{0.0};
    int contracts{0};
    std::string reason;
};

struct MarketData {
    std::string symbol;
    double currentPrice{0.0};
    double vwap{0.0};
    double high{0.0};
    double low{0.0};
    double close{0.0};
    double volume{0.0};
    std::string recentNewsSummary;
};

class Engine {
public:
    Engine(const config::AssetSpec& spec,
           ai_filter::IAIClient& ai,
           risk::Manager& risk);

    [[nodiscard]] std::optional<TradeSignal> evaluate(const MarketData& data);

    [[nodiscard]] bool aiGateAllows(const ai_filter::RegimeOutput& regime, SignalSide side) const;

private:
    const config::AssetSpec& spec_;
    ai_filter::IAIClient& ai_;
    risk::Manager& risk_;

    [[nodiscard]] double stopDistance(double price) const;
    [[nodiscard]] double computeVWAP(double high, double low, double close) const;
};

} // namespace trading::strategy
