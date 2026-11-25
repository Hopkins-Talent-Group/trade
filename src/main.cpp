#include "trading/config/Config.hpp"
#include "trading/config/SessionFilter.hpp"
#include "trading/ai_filter/Regime.hpp"
#include "trading/risk/Manager.hpp"
#include "trading/strategy/Engine.hpp"
#include "trading/execution/IBKR.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string asset = "MNQ";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--asset" && i + 1 < argc) {
            asset = argv[++i];
        }
    }
    if (asset != "MNQ" && asset != "MGC" && asset != "GC") {
        std::cerr << "Usage: " << argv[0] << " --asset MNQ|MGC|GC\n";
        return 2;
    }

    // C1 — Foundation: configuration & multi-asset support
    const auto config = trading::config::loadConfig("config/bot.properties");
    const auto& spec = trading::config::specFor(config.assets, asset);

    // C2 — AI Filter (stub; data-only, no execution)
    trading::ai_filter::StubAIClient aiClient;

    // C3 — Risk Manager (deterministic, no AI)
    trading::risk::Manager riskManager(trading::risk::RiskLimits{});

    // C4 — Strategy Engine (VWAP + S/R + breakout/retest + AI gate)
    trading::strategy::Engine engine(spec, aiClient, riskManager);

    // C5 — Execution (stub; bracket orders, isAutomated=true)
    trading::execution::StubGateway gateway;

    // C6 — Orchestration & health check
    const auto now = std::chrono::system_clock::now();
    const auto session = trading::config::sessionFilter(spec, now);
    const auto regime = trading::ai_filter::evaluateRegime(
        aiClient, asset, 0.0, 0.0, "");

    const bool tradingEnabled = session.isTradable && !riskManager.killSwitchArmed();
    const std::string status = tradingEnabled ? "ACTIVE" : "PAUSED";

    std::cout << "AI Regime: ["
              << trading::ai_filter::regimeToString(regime.regime) << " | "
              << trading::ai_filter::biasToString(regime.bias) << " | "
              << trading::ai_filter::riskLevelToString(regime.riskLevel)
              << "] | Risk Limit: [daily loss $"
              << riskManager.dailyLossUSD() << " / $150.00 | trades "
              << riskManager.tradesTaken() << "/5]"
              << " | Trading Status: " << status << "\n";

    // Run-once evaluation scaffold (integration testing entry point).
    if (tradingEnabled) {
        trading::strategy::MarketData md{};
        md.symbol = asset;
        md.currentPrice = 100.0;
        md.vwap = 99.0;
        md.high = 101.0;
        md.low = 98.0;
        md.close = 100.0;
        md.volume = 1000.0;
        md.recentNewsSummary = "";

        const auto signal = engine.evaluate(md);
        if (signal.has_value()) {
            const auto exec = gateway.submitBracket(*signal);
            if (!exec.accepted) {
                std::cerr << "Execution rejected: " << exec.reason << "\n";
            } else if (exec.positionClosed) {
                std::cerr << "Position closed: stop loss failed to attach\n";
                riskManager.armKillSwitch();
                gateway.disconnect();
            } else {
                std::cout << "Order submitted: " << exec.bracket.orderId
                          << " (isAutomated=" << exec.bracket.isAutomated << ")\n";
            }
        } else {
            std::cout << "No signal emitted\n";
        }
    }

    return 0;
}
