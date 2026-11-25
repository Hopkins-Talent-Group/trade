#include "trading/config/SessionFilter.hpp"

#include <ctime>

namespace trading::config {

TradingSession sessionFilter(
    const AssetSpec& spec,
    std::chrono::system_clock::time_point now) {

    TradingSession result{};
    result.sessionName = spec.session;

    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    const std::tm* nowUtc = std::gmtime(&nowTime);
    const int hour = nowUtc ? nowUtc->tm_hour : 0;

    if (spec.session == "us_regular") {
        // MNQ: US regular session 09:30-16:00 ET (approx UTC-5 => 14:30-21:00 UTC)
        if (hour >= 14 && hour < 21) {
            result.isTradable = true;
            result.reason = "MNQ US session open";
        } else {
            result.isTradable = false;
            result.reason = "MNQ outside US session";
        }
    } else if (spec.session == "twenty_four_hour" || spec.session == "overnight") {
        // Gold: 24-hour / overnight focus - always tradable in scaffold
        result.isTradable = true;
        result.reason = "Gold 24-hour session";
    } else {
        result.isTradable = false;
        result.reason = "Unknown session";
    }
    return result;
}

} // namespace trading::config
