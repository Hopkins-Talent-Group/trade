# C6 — Orchestration & Health Check

Part of Directive 001: Expand `mnq-robot` to Multi-Asset AI-Filtered Engine (MNQ + Gold).

## Scope
- Update `src/main.py`.
- Add an argument parser to run the bot for either `--asset MNQ` or `--asset MGC`.
- Implement a health check that prints: `AI Regime: [X] | Risk Limit: [Y] | Trading Status: ACTIVE/PAUSED`.
- Orchestrates C1–C5: loads config, instantiates AI filter, risk manager, strategy engine, and execution module; runs the strategy loop and executes approved signals.

## Dependencies
- C1–C5: all previous layers must be complete before C6 is implemented.
- C6 is the final layer; it wires everything together.

## Core Functions

### 1. Argument parser
- Required, mutually exclusive: `--asset MNQ` or `--asset MGC`.
- Validates asset against supported set before starting.

### 2. Main loop
- Loads C1 config for the selected asset.
- Instantiates C2 AI filter, C3 risk manager, C4 strategy engine, C5 execution.
- Runs: market data → C2 regime → C3 pre-check → C4 evaluate → C5 submit (if approved).
- Responds to C3 kill_switch by pausing trading.

### 3. Health check
- Prints on startup and periodically: `AI Regime: [X] | Risk Limit: [Y] | Trading Status: ACTIVE/PAUSED`.
- `[X]` = latest regime from C2 (e.g., `trending | long | low`).
- `[Y]` = remaining daily-loss allowance or trade-count remaining from C3.
- `ACTIVE` if all checks pass; `PAUSED` if kill switch engaged or limits breached.

## Contract
- `python src/main.py --asset MNQ` starts cleanly and prints the health line.
- `python src/main.py --asset MGC` starts cleanly and prints the health line.
- Graceful shutdown on kill_switch activation.

## Definition of Done
- Both `--asset MNQ` and `--asset MGC` accepted and start without errors.
- Health check line prints the expected format.
- Loop responds to kill_switch by setting `Trading Status: PAUSED`.
- All six layers (C1–C6) complete; architecture compiles; ready for integration testing.
