# C3 — Risk Management Layer

Part of Directive 001: Expand `mnq-robot` to Multi-Asset AI-Filtered Engine (MNQ + Gold).

## Scope
- Harden `src/risk/manager.py` with deterministic limits only.
- **No AI allowed here.** All decisions are rule-based.
- Max Daily Loss: $150 (both assets combined).
- Max Trades per Session: 5.
- Position Sizing: exact contract sizes based on the asset (1 MNQ vs 1 MGC).
- Kill Switch: method to immediately flatten all positions and disconnect IBKR if drawdown exceeds limits.

## Dependencies
- C1 (Foundation): per-asset position sizing, margin limits, session windows.

## Core Functions

### 1. Daily loss enforcement
- Tracks realized + unrealized P&L across MNQ and MGC combined.
- Rejects any new trade if cumulative daily loss would exceed $150.

### 2. Trade count enforcement
- Session counter resets per `session_filter` window.
- Rejects new trades once 5 trades are reached in a session.

### 3. Position sizing (`position_size(asset, account_equity, risk_per_trade)`)
- Calculates exact contract count per asset.
- 1 MNQ contract ≠ 1 MGC/GC contract notional; sizing differs accordingly.

### 4. Kill switch (`kill_switch()`)
- Immediately flattens all open positions.
- Cancels open orders.
- Disconnects IBKR.
- Sets trading status to PAUSED.

## Contract
- All functions deterministic; zero AI/LLM calls.
- Returns pass/fail + reason for each check.
- Consumed by C4 (pre-check before signal) and C5 (kill-switch integration).

## Definition of Done
- Daily-loss, trade-count, per-asset position sizing, and kill_switch unit-tested.
- All functions deterministic.
- Max daily loss capped at $150 combined across assets.
- Max 5 trades per session enforced.
