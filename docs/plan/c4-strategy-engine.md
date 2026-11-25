# C4 — Strategy Engine Layer

Part of Directive 001: Expand `mnq-robot` to Multi-Asset AI-Filtered Engine (MNQ + Gold).

## Scope
- Update `src/strategy/engine.py`.
- Keep existing VWAP + S/R + Breakout/Retest logic.
- Add a gate: before the strategy outputs a trade signal, it must check the AI Filter (C2). If AI says "high risk" or bias contradicts the signal, veto the trade.
- Adjust S/R calculation to handle Gold's specific volatility (do not use MNQ point distances for Gold).

## Dependencies
- C1 (Foundation): asset config, session filter.
- C2 (AI Filter): regime/bias/risk_level input for the gate.
- C3 (Risk Manager): deterministic pre-check (daily loss, trade count, position sizing).

## Core Functions

### 1. VWAP + S/R + Breakout/Retest (existing logic preserved)
- Computes VWAP, support/resistance levels, breakout and retest signals per asset.

### 2. Gold-specific S/R adjustment
- Volatility-scaled distances for Gold, independent of MNQ point values.
- MNQ point distances never applied to Gold calculations.

### 3. AI gate
- Before emitting a signal, query C2 `get_market_regime`.
- Veto if `risk_level == "high"`.
- Veto if `bias` contradicts the signal direction (e.g., signal = long but AI bias = short).
- Veto if C3 risk pre-check fails (daily-loss or trade-count limit would be breached).

## Contract
- `engine.evaluate(market_data)` → `Signal | None` (returns `None` when vetoed).
- Signal only emitted after C2 gate + C3 pre-check both pass.

## Definition of Done
- Strategy emits signal only after C2 gate and C3 pre-check pass.
- Gold S/R uses Gold-specific volatility (no MNQ point distances).
- Veto path returns `None` with reason logged.
