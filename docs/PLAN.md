# Implementation Plan: Multi-Asset AI-Filtered Engine (MNQ + Gold)

Source directive: `docs/COMMAND DIRECTIVE 001 - Expand mnq-robot to Multi-Asset.txt`

## Goal
Upgrade the IBKR execution scaffold to support both MNQ (Micro Nasdaq) and MGC/GC (Micro/Full Gold) using a hybrid architecture: deterministic Python execution with an LLM used strictly as a macro/regime filter.

---

## Categories of Action Plan

The directive's six required updates are grouped into six action categories. Each category owns a bounded set of core functions and exposes a clear contract to downstream categories.

| Category | Scope | Delivers |
|----------|-------|----------|
| **C1 — Foundation: Configuration & Multi-Asset Support** | `config/config.yaml`, session filter | Asset-specific parameters (tick size, tick value, margin limits) for MNQ and MGC; `session_filter` block (MNQ: US session only; Gold: 24-hour/overnight focus). |
| **C2 — AI Filter Layer** | `src/ai_filter/regime.py` | DeepSeek/OpenAI API wrapper (keys from `.env`); `get_market_regime(asset, current_price, vwap, recent_news_summary)` returning strict JSON `{"regime": "trending\|ranging\|volatile", "bias": "long\|short\|neutral", "risk_level": "low\|high"}`. Data-only: NO execution permissions. |
| **C3 — Risk Management Layer** | `src/risk/manager.py` | Deterministic limits only (NO AI): max daily loss $150 (both assets combined), max 5 trades/session, exact position sizing per asset (1 MNQ vs 1 MGC), kill switch (flatten all positions + disconnect IBKR on drawdown breach). |
| **C4 — Strategy Engine Layer** | `src/strategy/engine.py` | VWAP + S/R + Breakout/Retest logic preserved; AI gate (veto on "high risk" or bias contradicting signal); Gold-specific S/R volatility (no MNQ point distances for Gold). |
| **C5 — Execution Layer** | `src/execution/ibkr.py` | Bracket orders (entry + hard stop loss + take profit); immediate position close if stop loss fails to attach; `isAutomated=True` on all order payloads. |
| **C6 — Orchestration & Health Check** | `src/main.py` | Argument parser (`--asset MNQ\|MGC`); health check printing `AI Regime: [X] | Risk Limit: [Y] | Trading Status: ACTIVE/PAUSED`. |

---

## Topological Creation Order (Dependency Graph)

Features are created in topological order: a category is implemented only after every category it depends on is complete.

```text
C1 (Foundation)
   │
   ├───► C2 (AI Filter) ────┐
   │                        │
   ├───► C3 (Risk) ─────────┼──► C4 (Strategy) ───┐
   │                        │                     │
   └────────────────────────┴─────────────────────┼──► C5 (Execution) ───┐
                                                  │                      │
                                                  └────────────────────────┴──► C6 (main.py)
```

**Dependency rationale:**
- **C1** has no dependencies — it is the base. All other categories read `config.yaml` for asset parameters and session rules.
- **C2** depends on **C1** (asset names, API key source). It is otherwise standalone and produces data only.
- **C3** depends on **C1** (per-asset position sizing, margin limits). It is deterministic and consumes no AI output.
- **C4** depends on **C1** (asset config), **C2** (AI gate input), and **C3** (risk veto pre-checks). It must not emit a trade signal until C2 and C3 approve.
- **C5** depends on **C1** (order payload tags), **C3** (kill-switch disconnect, position flattening). It receives approved signals from C4.
- **C6** depends on **C1–C5** — it orchestrates all modules and prints the aggregated health status.

**Implementation sequence:** C1 → C2 → C3 → C4 → C5 → C6

---

## Stretched Core Functions per Category (Topological Detail)

### C1 — Foundation: Configuration & Multi-Asset Support
- Define `assets.MNQ`: tick size, tick value, margin limit, session = US regular hours.
- Define `assets.MGC` (Micro Gold) / `GC` (Full Gold): tick size, tick value, margin limit, session = 24-hour/overnight focus.
- Implement `session_filter(asset, current_time)` → boolean tradable window.
- Load all parameters from `.env` where applicable; fail fast on missing required keys.
- **Contract:** `config.get_asset_params(asset)` returns a complete parameter set; `config.session_filter(asset, ts)` returns tradable/not-tradable.

### C2 — AI Filter Layer
- Connect to DeepSeek or OpenAI API; keys read exclusively from `.env`.
- Implement `get_market_regime(asset, current_price, vwap, recent_news_summary)`.
- Parse and validate strict JSON output; reject/fallback on malformed responses.
- Enforce data-only boundary: module exposes zero order/send/execute functions.
- **Contract:** returns `{"regime", "bias", "risk_level"}`; raises on invalid schema; never touches IBKR.

### C3 — Risk Management Layer
- `max_daily_loss`: $150 hard ceiling across MNQ + MGC combined.
- `max_trades_per_session`: 5; session counter resets per session_filter window.
- `position_size(asset, account_equity, risk_per_trade)` → exact contract count (1 MNQ ≠ 1 MGC notional).
- `kill_switch()` → flatten all open positions, cancel open orders, disconnect IBKR, set `Trading Status: PAUSED`.
- **Contract:** all functions deterministic; zero AI/LLM calls; returns pass/fail + reason.

### C4 — Strategy Engine Layer
- Preserve existing VWAP + support/resistance + breakout/retest logic.
- S/R adjustment for Gold: volatility-scaled distances independent of MNQ point values.
- AI gate: before emitting a signal, query C2; veto if `risk_level == "high"` or `bias` contradicts signal direction.
- Risk pre-check via C3: reject if daily-loss or trade-count limits would be breached.
- **Contract:** `engine.evaluate(market_data)` → `Signal | None` (None when vetoed).

### C5 — Execution Layer
- Every entry transmitted as a bracket order: entry + hard stop loss + take profit.
- Stop-loss attachment verification; if attach fails → immediately close the position.
- All order payloads tagged `isAutomated=True` (compliance).
- Integration with C3 kill_switch on drawdown breach.
- **Contract:** `execution.submit(signal)` → order ID or rejection reason; guarantees bracket or flat.

### C6 — Orchestration & Health Check
- CLI argument parser: `--asset MNQ` or `--asset MGC` (required, mutually exclusive).
- Main loop: load C1 config → instantiate C2/C3/C4/C5 → run strategy → execute approved signals.
- Health check output: `AI Regime: [X] | Risk Limit: [Y] | Trading Status: ACTIVE/PAUSED`.
- Graceful shutdown on kill_switch activation.
- **Contract:** `python src/main.py --asset MNQ` and `--asset MGC` both start cleanly and print the health line.

---

## Constraints (from directive)
- AI Filter returns data only — **NO execution permissions**.
- Risk Manager is deterministic — **NO AI**.
- Do not write full AI prompt logic yet; build the API wrapper + data flow only.
- Gold S/R must **not** use MNQ point distances.
- Max Daily Loss: $150 (both assets combined); Max Trades per Session: 5.
- All order payloads must include `isAutomated=True`.

---

## Definition of Done (per category)
| Category | Done when |
|----------|-----------|
| C1 | `config.yaml` compiles; MNQ and MGC parameter sets load; `session_filter` returns correct windows for both assets. |
| C2 | `regime.py` imports without API keys present (graceful degradation); returns valid strict JSON for a mocked response; no execution functions exist in module. |
| C3 | Daily-loss, trade-count, per-asset position sizing, and kill_switch unit-tested; all deterministic. |
| C4 | Strategy emits signal only after C2 gate + C3 pre-check pass; Gold S/R uses Gold-specific volatility; veto path returns `None`. |
| C5 | Bracket order submitted with `isAutomated=True`; failed stop attach triggers immediate position close. |
| C6 | `python src/main.py --asset MNQ` and `--asset MGC` start; health check line prints; loop responds to kill_switch by pausing. |

**Project done:** all six categories complete, architecture compiles, ready for integration testing.
