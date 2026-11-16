#include "trading/RegimeFilter.hpp"

namespace trading {

MacroRegime classifyRegime(const MarketObservation& observation) {
    if (observation.riskDeleveraging) {
        return MacroRegime::RiskOff;
    }
    if (observation.dominantDriver == "rates" &&
        observation.yieldTrend == "rising") {
        return MacroRegime::RiskOff;
    }
    if (observation.dominantDriver == "dollar" &&
        observation.dollarStrength == "strong") {
        return MacroRegime::RiskOff;
    }
    if (observation.dominantDriver == "liquidity" &&
        observation.riskDeleveraging == false) {
        return MacroRegime::RiskOn;
    }
    return MacroRegime::Transitional;
}

bool isRegimeAlignedWithTrade(MacroRegime regime, bool isLong) {
    if (isLong) {
        return regime == MacroRegime::RiskOn;
    }
    return regime == MacroRegime::RiskOff;
}

double adjustSizeForRegime(MacroRegime regime, double baseSize, bool isLong) {
    if (regime == MacroRegime::RiskOff && isLong) {
        return baseSize * 0.5;
    }
    if (regime == MacroRegime::RiskOn && !isLong) {
        return baseSize * 0.5;
    }
    if (regime == MacroRegime::Transitional) {
        return baseSize * 0.75;
    }
    return baseSize;
}

double tightenStopForRegime(MacroRegime regime, double baseStop, bool isLong) {
    if (regime == MacroRegime::RiskOff) {
        return baseStop * 0.75;
    }
    if (regime == MacroRegime::Transitional) {
        return baseStop * 0.85;
    }
    return baseStop;
}

} // namespace trading