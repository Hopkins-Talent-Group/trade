# C1 — Foundation: Configuration & Multi-Asset Support

Part of Directive 001: Expand `mnq-robot` to Multi-Asset AI-Filtered Engine (MNQ + Gold).

## Scope
- Update `config/config.yaml` with asset-specific parameters for `MNQ` and `MGC`/`GC` (tick size, tick value, margin limits).
- Add a `session_filter` logic block: MNQ = US session only; Gold = 24-hour/overnight focus.
- No dependencies on other layers. This is the base layer; all downstream layers read from it.

## Core Functions

### 1. Asset parameter registry
Define per-asset parameters in `config/config.yaml`:

| Asset | Tick Size | Tick Value | Margin Limit | Session |
|-------|-----------|------------|--------------|---------|
| MNQ   | (per IBKR spec) | (per IBKR spec) | per contract | US regular hours only |
| MGC   | (per IBKR spec) | (per IBKR spec) | per contract | 24-hour / overnight focus |
| GC    | (per IBKR spec) | (per IBKR spec) | per contract | 24-hour / overnight focus |

### 2. `session_filter(asset, current_time)` → boolean
- Returns `True` if the asset is tradable at `current_time`.
- MNQ: tradable only during US regular session.
- Gold (MGC/GC): tradable during 24-hour/overnight sessions.
- Must be deterministic and config-driven (no hardcoded timestamps outside config).

### 3. Config loader
- Loads `config.yaml` at startup.
- Fails fast on missing required keys (tick size, tick value, margin limit per asset).
- Reads API key paths from `.env` where applicable.

## Contract
- `config.get_asset_params(asset)` → complete parameter set for `MNQ`, `MGC`, or `GC`.
- `config.session_filter(asset, timestamp)` → `True`/`False`.
- All parameters consumed by C2–C6; no layer hardcodes asset values.

## Definition of Done
- `config.yaml` parses without errors.
- MNQ and MGC/GC parameter sets load correctly.
- `session_filter` returns correct windows for both asset classes.
- C1 compiles/loads independently before any other layer is implemented.
