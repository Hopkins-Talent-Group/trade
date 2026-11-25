// TradingFrameworkTests.cpp
// Comprehensive GoogleTest unit tests for the C1-C4 trading framework layers.

#include "trading/ai_filter/Regime.hpp"
#include "trading/config/AssetSpec.hpp"
#include "trading/config/SessionFilter.hpp"
#include "trading/risk/Manager.hpp"
#include "trading/strategy/Engine.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <optional>
#include <string>

// ---------------------------------------------------------------------------
// Helper: build a UTC time_point from a known date/hour/minute
// 2026-01-05 (Monday) is used as a stable reference date.
// ---------------------------------------------------------------------------
namespace {

std::chrono::system_clock::time_point makeUTC(int hour, int minute = 0) {
    std::tm tm{};
    tm.tm_year = 126; // 2026
    tm.tm_mon  = 0;   // January
    tm.tm_mday = 5;
    tm.tm_hour = hour;
    tm.tm_min  = minute;
    tm.tm_sec  = 0;
    tm.tm_isdst = 0;
    std::time_t t = timegm(&tm);
    return std::chrono::system_clock::from_time_t(t);
}

} // namespace

// ===========================================================================
// C1 — Asset Registry
// ===========================================================================

class AssetRegistryTest : public ::testing::Test {
protected:
    trading::config::AssetRegistry registry = trading::config::loadDefaultRegistry();
};

TEST_F(AssetRegistryTest, MNQ_TickSize) {
    const auto spec = trading::config::specFor(registry, "MNQ");
    EXPECT_DOUBLE_EQ(spec.tickSize, 0.25);
}

TEST_F(AssetRegistryTest, MNQ_TickValue) {
    const auto spec = trading::config::specFor(registry, "MNQ");
    EXPECT_DOUBLE_EQ(spec.tickValue, 0.50);
}

TEST_F(AssetRegistryTest, MNQ_Margin) {
    const auto spec = trading::config::specFor(registry, "MNQ");
    EXPECT_DOUBLE_EQ(spec.marginLimit, 500.0);
}

TEST_F(AssetRegistryTest, MGC_TickSize) {
    const auto spec = trading::config::specFor(registry, "MGC");
    EXPECT_DOUBLE_EQ(spec.tickSize, 0.10);
}

TEST_F(AssetRegistryTest, MGC_TickValue) {
    const auto spec = trading::config::specFor(registry, "MGC");
    EXPECT_DOUBLE_EQ(spec.tickValue, 1.00);
}

TEST_F(AssetRegistryTest, MGC_Margin) {
    const auto spec = trading::config::specFor(registry, "MGC");
    EXPECT_DOUBLE_EQ(spec.marginLimit, 1000.0);
}

TEST_F(AssetRegistryTest, GC_TickSize) {
    const auto spec = trading::config::specFor(registry, "GC");
    EXPECT_DOUBLE_EQ(spec.tickSize, 0.10);
}

TEST_F(AssetRegistryTest, GC_TickValue) {
    const auto spec = trading::config::specFor(registry, "GC");
    EXPECT_DOUBLE_EQ(spec.tickValue, 10.00);
}

TEST_F(AssetRegistryTest, GC_Margin) {
    const auto spec = trading::config::specFor(registry, "GC");
    EXPECT_DOUBLE_EQ(spec.marginLimit, 5000.0);
}

TEST_F(AssetRegistryTest, UnknownSymbolFallsBackToMNQ) {
    const auto spec = trading::config::specFor(registry, "UNKNOWN");
    EXPECT_EQ(spec.symbol, "MNQ");
    EXPECT_DOUBLE_EQ(spec.tickSize, 0.25);
}

// ===========================================================================
// C1 — Session Filter
// ===========================================================================

class SessionFilterTest : public ::testing::Test {
protected:
    trading::config::AssetRegistry registry = trading::config::loadDefaultRegistry();
};

TEST_F(SessionFilterTest, MNQ_TradableAt15UTC) {
    // 15:00 UTC = 10:00 AM ET — US regular session
    const auto mnq = trading::config::specFor(registry, "MNQ");
    auto result = trading::config::sessionFilter(mnq, makeUTC(15, 0));
    EXPECT_TRUE(result.isTradable) << "Expected MNQ tradable at 15:00 UTC; reason: " << result.reason;
}

TEST_F(SessionFilterTest, MNQ_NotTradableAt02UTC) {
    // 02:00 UTC = 10:00 PM ET — after hours
    const auto mnq = trading::config::specFor(registry, "MNQ");
    auto result = trading::config::sessionFilter(mnq, makeUTC(2, 0));
    EXPECT_FALSE(result.isTradable) << "Expected MNQ NOT tradable at 02:00 UTC";
}

TEST_F(SessionFilterTest, MGC_TradableAt02UTC) {
    // MGC is a 24-hour product
    const auto mgc = trading::config::specFor(registry, "MGC");
    auto result = trading::config::sessionFilter(mgc, makeUTC(2, 0));
    EXPECT_TRUE(result.isTradable) << "Expected MGC tradable at 02:00 UTC; reason: " << result.reason;
}

TEST_F(SessionFilterTest, GC_TradableAt02UTC) {
    // GC is a 24-hour product
    const auto gc = trading::config::specFor(registry, "GC");
    auto result = trading::config::sessionFilter(gc, makeUTC(2, 0));
    EXPECT_TRUE(result.isTradable) << "Expected GC tradable at 02:00 UTC; reason: " << result.reason;
}

// ===========================================================================
// C3 — Risk Manager
// NOTE: checkDailyLoss(double) and checkTradeCount(int) take parameters
//       as defined in the actual header/implementation.
// ===========================================================================

class RiskManagerTest : public ::testing::Test {
protected:
    trading::risk::RiskLimits limits{};  // maxDailyLossUSD=150, maxTradesPerSession=5

    // fresh manager for each test
    trading::risk::Manager mgr{limits};
};

TEST_F(RiskManagerTest, CheckDailyLoss_PassesWhenNoLossRecorded) {
    auto check = mgr.checkDailyLoss();
    EXPECT_TRUE(check.pass) << check.reason;
}

TEST_F(RiskManagerTest, CheckDailyLoss_FailsWhenLossReachesLimit) {
    // Record a losing trade to accumulate dailyLossUSD_ = -150
    mgr.recordTrade(-150.0);
    // dailyLossUSD_ is now -150; checkDailyLoss(0) should fail
    auto check = mgr.checkDailyLoss();
    EXPECT_FALSE(check.pass) << "Expected daily loss check to fail at -150 USD";
}

TEST_F(RiskManagerTest, CheckTradeCount_PassesWhenZeroTrades) {
    auto check = mgr.checkTradeCount();
    EXPECT_TRUE(check.pass) << check.reason;
}

TEST_F(RiskManagerTest, CheckTradeCount_FailsWhenAtLimit) {
    // maxTradesPerSession = 5; record 5 trades then check
    for (int i = 0; i < 5; ++i) mgr.recordTrade(0.0);
    auto check = mgr.checkTradeCount();
    EXPECT_FALSE(check.pass) << "Expected trade count check to fail at 5 trades";
}

TEST_F(RiskManagerTest, SizePosition_OkForMNQWithValidEquity) {
    trading::config::AssetRegistry registry = trading::config::loadDefaultRegistry();
    const auto mnq = trading::config::specFor(registry, "MNQ");
    auto result = mgr.sizePosition(mnq, 100000.0, 1000.0);
    EXPECT_TRUE(result.ok) << result.reason;
    EXPECT_GE(result.contracts, 1);
}

TEST_F(RiskManagerTest, SizePosition_ReturnsFalseWhenKillSwitchArmed) {
    trading::config::AssetRegistry registry = trading::config::loadDefaultRegistry();
    const auto mnq = trading::config::specFor(registry, "MNQ");
    mgr.armKillSwitch();
    auto result = mgr.sizePosition(mnq, 100000.0, 1000.0);
    EXPECT_FALSE(result.ok) << "Expected sizePosition to fail with kill switch armed";
}

TEST_F(RiskManagerTest, ArmKillSwitch_CausesAllCheckToFail) {
    mgr.armKillSwitch();
    EXPECT_TRUE(mgr.killSwitchArmed());
    auto lossCheck  = mgr.checkDailyLoss();
    auto countCheck = mgr.checkTradeCount();
    EXPECT_FALSE(lossCheck.pass)  << "checkDailyLoss should fail with kill switch armed";
    EXPECT_FALSE(countCheck.pass) << "checkTradeCount should fail with kill switch armed";
}

TEST_F(RiskManagerTest, ResetSession_ClearsLossAndTradeCount) {
    mgr.recordTrade(-50.0);
    mgr.recordTrade(-40.0);
    EXPECT_DOUBLE_EQ(mgr.dailyLossUSD(), -90.0);
    EXPECT_EQ(mgr.tradesTaken(), 2);

    mgr.resetSession();

    EXPECT_DOUBLE_EQ(mgr.dailyLossUSD(), 0.0);
    EXPECT_EQ(mgr.tradesTaken(), 0);
    EXPECT_FALSE(mgr.killSwitchArmed());
}

TEST_F(RiskManagerTest, RecordTrade_AccumulatesLoss) {
    mgr.recordTrade(-60.0);
    mgr.recordTrade(-40.0);
    EXPECT_DOUBLE_EQ(mgr.dailyLossUSD(), -100.0);
    EXPECT_EQ(mgr.tradesTaken(), 2);
}

TEST_F(RiskManagerTest, RecordTrade_AutoArmsKillSwitchAtLimit) {
    // Single trade that pushes to -150 should auto-arm kill switch
    mgr.recordTrade(-150.0);
    EXPECT_TRUE(mgr.killSwitchArmed())
        << "Kill switch should auto-arm when dailyLoss reaches -150";
}

// ===========================================================================
// C2 — AI Filter
// ===========================================================================

TEST(AIFilterTest, StubAIClientReturnsValidOutput) {
    trading::ai_filter::StubAIClient stub;
    auto result = stub.getMarketRegime("MNQ", 20000.0, 19950.0, "no news");
    // Must not crash and may or may not return a value
    // (StubAIClient returns a value per implementation)
    EXPECT_TRUE(result.has_value());
}

TEST(AIFilterTest, EvaluateRegime_ReturnsDefaultSafeValuesWhenClientReturnsNullopt) {
    // A client that always returns nullopt
    class NullAIClient : public trading::ai_filter::IAIClient {
    public:
        std::optional<trading::ai_filter::RegimeOutput> getMarketRegime(
            const std::string&, double, double, const std::string&) override {
            return std::nullopt;
        }
    };

    NullAIClient nullClient;
    auto regime = trading::ai_filter::evaluateRegime(
        nullClient, "MNQ", 20000.0, 19950.0, "");

    // Fail-safe defaults: Ranging, Neutral, Low
    EXPECT_EQ(regime.regime,    trading::ai_filter::Regime::Ranging);
    EXPECT_EQ(regime.bias,      trading::ai_filter::Bias::Neutral);
    EXPECT_EQ(regime.riskLevel, trading::ai_filter::RiskLevel::Low);
}

TEST(AIFilterTest, RegimeToString_Trending) {
    EXPECT_EQ(trading::ai_filter::regimeToString(trading::ai_filter::Regime::Trending), "trending");
}

TEST(AIFilterTest, RegimeToString_Ranging) {
    EXPECT_EQ(trading::ai_filter::regimeToString(trading::ai_filter::Regime::Ranging), "ranging");
}

TEST(AIFilterTest, RegimeToString_Volatile) {
    EXPECT_EQ(trading::ai_filter::regimeToString(trading::ai_filter::Regime::Volatile), "volatile");
}

TEST(AIFilterTest, BiasToString_Long) {
    EXPECT_EQ(trading::ai_filter::biasToString(trading::ai_filter::Bias::Long), "long");
}

TEST(AIFilterTest, BiasToString_Short) {
    EXPECT_EQ(trading::ai_filter::biasToString(trading::ai_filter::Bias::Short), "short");
}

TEST(AIFilterTest, BiasToString_Neutral) {
    EXPECT_EQ(trading::ai_filter::biasToString(trading::ai_filter::Bias::Neutral), "neutral");
}

TEST(AIFilterTest, RiskLevelToString_Low) {
    EXPECT_EQ(trading::ai_filter::riskLevelToString(trading::ai_filter::RiskLevel::Low), "low");
}

TEST(AIFilterTest, RiskLevelToString_High) {
    EXPECT_EQ(trading::ai_filter::riskLevelToString(trading::ai_filter::RiskLevel::High), "high");
}

// ===========================================================================
// C4 — Strategy Engine (using MockAIClient)
// ===========================================================================

namespace {

class MockAIClient : public trading::ai_filter::IAIClient {
public:
    trading::ai_filter::RegimeOutput fixedOutput;

    std::optional<trading::ai_filter::RegimeOutput> getMarketRegime(
        const std::string&, double, double, const std::string&) override {
        return fixedOutput;
    }
};

} // namespace

class StrategyEngineTest : public ::testing::Test {
protected:
    trading::config::AssetRegistry registry = trading::config::loadDefaultRegistry();
    trading::risk::RiskLimits      limits{};        // default: 150 USD loss, 5 trades
    trading::risk::Manager         mgr{limits};
    MockAIClient                   mockAI;

    // Market data with price above VWAP → Long signal
    trading::strategy::MarketData longBiasData{
        "MNQ",    // symbol
        20100.0,  // currentPrice > vwap → Long
        20000.0,  // vwap
        20200.0,  // high
        19900.0,  // low
        20100.0,  // close
        5000.0,   // volume
        20300.0,  // sessionHigh
        19800.0,  // sessionLow
        50.0,     // atr
        100000.0, // accountEquityUSD
        1000.0,   // riskPerTradeUSD
        "positive outlook"  // recentNewsSummary
    };

    // Set up a neutral, low-risk AI output by default
    void SetUp() override {
        mockAI.fixedOutput.regime    = trading::ai_filter::Regime::Ranging;
        mockAI.fixedOutput.bias      = trading::ai_filter::Bias::Neutral;
        mockAI.fixedOutput.riskLevel = trading::ai_filter::RiskLevel::Low;
    }
};

TEST_F(StrategyEngineTest, ReturnsNulloptWhenKillSwitchArmed) {
    mgr.armKillSwitch();
    // Store spec by value to avoid dangling-reference warning from GCC -Wdangling-reference
    trading::config::AssetSpec spec = trading::config::specFor(registry, "MNQ");
    trading::strategy::Engine engine(spec, mockAI, mgr);
    auto signal = engine.evaluate(longBiasData);
    EXPECT_FALSE(signal.has_value()) << "Expected nullopt when kill switch is armed";
}

TEST_F(StrategyEngineTest, ReturnsNulloptWhenAIRiskLevelIsHigh) {
    mockAI.fixedOutput.riskLevel = trading::ai_filter::RiskLevel::High;
    trading::config::AssetSpec spec = trading::config::specFor(registry, "MNQ");
    trading::strategy::Engine engine(spec, mockAI, mgr);
    auto signal = engine.evaluate(longBiasData);
    EXPECT_FALSE(signal.has_value()) << "Expected nullopt when AI risk level is High";
}

TEST_F(StrategyEngineTest, ReturnsNulloptWhenAIBiasContradictsLongSignal) {
    // Price above VWAP → Long signal; AI says Short bias → veto
    mockAI.fixedOutput.bias = trading::ai_filter::Bias::Short;
    trading::config::AssetSpec spec = trading::config::specFor(registry, "MNQ");
    trading::strategy::Engine engine(spec, mockAI, mgr);
    auto signal = engine.evaluate(longBiasData);
    EXPECT_FALSE(signal.has_value()) << "Expected nullopt when AI bias contradicts Long signal";
}

TEST_F(StrategyEngineTest, ReturnsValidSignalWhenPriceAboveVWAPAndAINeutralLow) {
    // AI is neutral/low-risk, price above VWAP → valid Long signal
    trading::config::AssetSpec spec = trading::config::specFor(registry, "MNQ");
    trading::strategy::Engine engine(spec, mockAI, mgr);
    auto signal = engine.evaluate(longBiasData);
    ASSERT_TRUE(signal.has_value()) << "Expected a valid signal; got nullopt";
    EXPECT_EQ(signal->side, trading::strategy::SignalSide::Long);
}

TEST_F(StrategyEngineTest, LongSignal_StopBelowEntry) {
    trading::config::AssetSpec spec = trading::config::specFor(registry, "MNQ");
    trading::strategy::Engine engine(spec, mockAI, mgr);
    auto signal = engine.evaluate(longBiasData);
    ASSERT_TRUE(signal.has_value());
    EXPECT_LT(signal->stop, signal->entry)
        << "Stop should be below entry for Long signal";
}

TEST_F(StrategyEngineTest, LongSignal_TargetAboveEntry_TwoToOneRR) {
    trading::config::AssetSpec spec = trading::config::specFor(registry, "MNQ");
    trading::strategy::Engine engine(spec, mockAI, mgr);
    auto signal = engine.evaluate(longBiasData);
    ASSERT_TRUE(signal.has_value());
    EXPECT_GT(signal->target, signal->entry)
        << "Target should be above entry for Long signal";

    // Verify 2:1 R:R: target dist = 2 * stop dist
    const double stopDist   = signal->entry - signal->stop;
    const double targetDist = signal->target - signal->entry;
    EXPECT_NEAR(targetDist, 2.0 * stopDist, 1e-9)
        << "Expected 2:1 risk/reward ratio";
}

TEST_F(StrategyEngineTest, GC_SignalUsesDifferentStopDistanceThanMNQ) {
    // Build a GC engine and an MNQ engine with the same price data,
    // then verify the stop distances differ.
    trading::strategy::MarketData gcData = longBiasData;
    gcData.symbol = "GC";

    // Store specs by value to avoid GCC -Wdangling-reference false positive
    trading::config::AssetSpec mnqSpec = trading::config::specFor(registry, "MNQ");
    trading::config::AssetSpec gcSpec  = trading::config::specFor(registry, "GC");

    trading::risk::Manager mnqMgr{limits};
    trading::risk::Manager gcMgr{limits};

    trading::strategy::Engine mnqEngine(mnqSpec, mockAI, mnqMgr);
    trading::strategy::Engine gcEngine(gcSpec,  mockAI, gcMgr);

    auto mnqSignal = mnqEngine.evaluate(longBiasData);
    auto gcSignal  = gcEngine.evaluate(gcData);

    ASSERT_TRUE(mnqSignal.has_value()) << "MNQ signal should be valid";
    ASSERT_TRUE(gcSignal.has_value())  << "GC signal should be valid";

    const double mnqStopDist = mnqSignal->entry - mnqSignal->stop;
    const double gcStopDist  = gcSignal->entry  - gcSignal->stop;

    EXPECT_NE(mnqStopDist, gcStopDist)
        << "GC stop distance (" << gcStopDist
        << ") should differ from MNQ stop distance (" << mnqStopDist << ")";
}
