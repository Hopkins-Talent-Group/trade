# Implementation Plan: Multi-Asset AI-Filtered Engine (MNQ + Gold)

Source directive: `docs/COMMAND DIRECTIVE 001 - Expand mnq-robot to Multi-Asset.txt`

## Goal
Upgrade the IBKR execution scaffold to support both MNQ (Micro Nasdaq) and MGC/GC (Micro/Full Gold) using a hybrid architecture: deterministic Python execution with an LLM used strictly as a macro/regime filter.

## Files to create / modify

### New files
1. `config/config.yaml` — add asset-specific parameters for MNQ and MGC (tick size, tick value, margin limits) + `session_filter` block
2. `src/ai_filter/regime.py` — AI filter module wrapping DeepSeek/OpenAI API; `get_market_regime(asset, current_price, vwap, recent_news_summary)` returning strict JSON `{"regime", "bias", "risk_level"}`
3. `.env` template — API keys for DeepSeek or OpenAI (no secrets committed)

### Modified files
4. `src/strategy/engine.py` — keep VWAP + S/R + Breakout/Retest; add AI gate (veto on "high risk" or contradicting bias); Gold-specific S/R volatility
5. `src/risk/manager.py` — deterministic limits: max daily loss $150 (combined), max 5 trades/session, exact position sizing per asset, kill switch (flatten + disconnect IBKR)
6. `src/execution/ibkr.py` — bracket orders (entry + hard stop + take profit), close position if stop attach fails, `isAutomated=True` on all payloads
7. `src/main.py` — argument parser `--asset MNQ|MGC`; health check `AI Regime: [X] | Risk Limit: [Y] | Trading Status: ACTIVE/PAUSED`

## Implementation order
1. Update `config/config.yaml` with MNQ/MGC params + session_filter
2. Create `src/ai_filter/regime.py` API wrapper (data-only, no execution)
3. Harden `src/risk/manager.py` (deterministic limits + kill switch)
4. Update `src/strategy/engine.py` (AI gate + Gold S/R)
5. Harden `src/execution/ibkr.py` (bracket orders + isAutomated)
6. Update `src/main.py` (arg parser + health check)
7. Verify architecture compiles; ready for integration testing

## Constraints (from directive)
- AI Filter returns data only — NO execution permissions
- Risk Manager is deterministic — NO AI
- Do not write full AI prompt logic yet; build API wrapper + data flow only
- Gold S/R must not use MNQ point distances
- Max Daily Loss: $150 both assets combined; Max Trades per Session: 5

## Definition of done
- Code compiles/runs without errors
- `--asset MNQ` and `--asset MGC` both accepted
- Health check prints expected status line
- Ready for integration testing
