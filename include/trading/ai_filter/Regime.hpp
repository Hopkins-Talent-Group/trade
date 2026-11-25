#pragma once

#include <optional>
#include <string>

namespace trading::ai_filter {

enum class Regime { Trending, Ranging, Volatile };
enum class Bias { Long, Short, Neutral };
enum class RiskLevel { Low, High };

struct RegimeOutput {
    Regime regime{Regime::Ranging};
    Bias bias{Bias::Neutral};
    RiskLevel riskLevel{RiskLevel::Low};
};

[[nodiscard]] std::string regimeToString(Regime regime);
[[nodiscard]] std::string biasToString(Bias bias);
[[nodiscard]] std::string riskLevelToString(RiskLevel riskLevel);

class IAIClient {
public:
    virtual ~IAIClient() = default;
    [[nodiscard]] virtual std::optional<RegimeOutput> getMarketRegime(
        const std::string& asset,
        double currentPrice,
        double vwap,
        const std::string& recentNewsSummary) = 0;
};

class StubAIClient : public IAIClient {
public:
    [[nodiscard]] std::optional<RegimeOutput> getMarketRegime(
        const std::string& asset,
        double currentPrice,
        double vwap,
        const std::string& recentNewsSummary) override;
};

[[nodiscard]] RegimeOutput evaluateRegime(
    IAIClient& client,
    const std::string& asset,
    double currentPrice,
    double vwap,
    const std::string& recentNewsSummary);

} // namespace trading::ai_filter
