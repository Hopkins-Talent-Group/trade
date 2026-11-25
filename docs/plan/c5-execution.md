# C5 — Execution Layer

Part of Directive 001: Expand `mnq-robot` to Multi-Asset AI-Filtered Engine (MNQ + Gold).

## Scope
- Harden `src/execution/ibkr.py`.
- All entries sent as Bracket Orders (Entry + Hard Stop Loss + Take Profit).
- If the Stop Loss fails to attach, the script must immediately close the position.
- Include `isAutomated=True` tags on all order payloads (for compliance).
- Integrates with C3 kill switch on drawdown breach.

## Dependencies
- C1 (Foundation): `isAutomated` tag source, asset config.
- C3 (Risk Manager): kill_switch (flatten + disconnect IBKR), position flattening on SL failure.
- C4 (Strategy Engine): receives approved `Signal` objects to execute.

## Core Functions

### 1. Bracket order submission (`submit(signal)`)
- Every entry transmitted as a bracket order: entry + hard stop loss + take profit.
- All order payloads tagged `isAutomated=True`.

### 2. Stop-loss attachment verification
- Confirms the stop loss is attached to the position.
- If the stop loss fails to attach → immediately close the position.

### 3. Kill switch integration
- On drawdown breach, invoke C3 `kill_switch()` to flatten all positions and disconnect IBKR.

## Contract
- `execution.submit(signal)` → order ID or rejection reason.
- Guarantees bracket order or flat position (no naked entries without stops).

## Definition of Done
- Bracket order submitted with `isAutomated=True` on every entry.
- Failed stop attach triggers immediate position close.
- Kill switch flattens all positions and disconnects IBKR.
