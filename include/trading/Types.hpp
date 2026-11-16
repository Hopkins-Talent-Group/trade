#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace trading {

enum class MacroRegime {
    RiskOn,
    RiskOff,
    Transitional
};

enum class OrderType {
    DayOrder,
    GoodTilCanceled,
    StopLimit,
    ConditionalGTC,
    AtTargetPrice
};

struct FeeAdjustment {
    double commission{0.0};
    double slippage{0.0};
    double total{0.0};
};

struct RRCalculation {
    double grossR{0.0};
    double netR{0.0};
    double entry{0.0};
    double stop{0.0};
    double target{0.0};
    FeeAdjustment costs{};
};

struct InvalidationPoint {
    double price{0.0};
    std::string condition{};
    bool isMacroChange{false};
};

struct DecisionMemo {
    std::string hypothesis{};
    InvalidationPoint invalidation;
    double feeAdjustedRR{0.0};
    struct {
        int positionSize{0};
        int maxLoss{0};
    } sizeCap;
};

struct TradeEvidence {
    std::string source{};
    std::string timestamp{};
    double confidence{0.0};
    bool known{false};
    std::string assumptions{};
};

struct KillSwitch {
    int dailyLossCap{0};
    int maxPositionCount{0};
    bool circuitBreaker{false};
};

struct MarketObservation {
    std::string dominantDriver{};
    MacroRegime regime{MacroRegime::Transitional};
    std::string dollarStrength{};
    std::string yieldTrend{};
    bool riskDeleveraging{false};
};

} // namespace trading