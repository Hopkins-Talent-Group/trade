#include "trading/ai_filter/Regime.hpp"

namespace trading::ai_filter {

std::string regimeToString(Regime regime) {
    switch (regime) {
        case Regime::Trending: return "trending";
        case Regime::Ranging: return "ranging";
        case Regime::Volatile: return "volatile";
    }
    return "ranging";
}

std::string biasToString(Bias bias) {
    switch (bias) {
        case Bias::Long: return "long";
        case Bias::Short: return "short";
        case Bias::Neutral: return "neutral";
    }
    return "neutral";
}

std::string riskLevelToString(RiskLevel riskLevel) {
    switch (riskLevel) {
        case RiskLevel::Low: return "low";
        case RiskLevel::High: return "high";
    }
    return "low";
}

std::optional<RegimeOutput> StubAIClient::getMarketRegime(
    const std::string& asset,
    double currentPrice,
    double vwap,
    const std::string& recentNewsSummary) {

    (void)asset;
    (void)currentPrice;
    (void)vwap;
    (void)recentNewsSummary;

    // Stub: returns a sample regime. Replace with real DeepSeek/OpenAI API call.
    RegimeOutput out{};
    out.regime = Regime::Ranging;
    out.bias = Bias::Neutral;
    out.riskLevel = RiskLevel::Low;
    return out;
}

RegimeOutput evaluateRegime(
    IAIClient& client,
    const std::string& asset,
    double currentPrice,
    double vwap,
    const std::string& recentNewsSummary) {

    auto opt = client.getMarketRegime(asset, currentPrice, vwap, recentNewsSummary);
    if (opt.has_value()) {
        return *opt;
    }
    // Fail-safe default when the API is unavailable.
    RegimeOutput out{};
    out.regime = Regime::Ranging;
    out.bias = Bias::Neutral;
    out.riskLevel = RiskLevel::Low;
    return out;
}

} // namespace trading::ai_filter
