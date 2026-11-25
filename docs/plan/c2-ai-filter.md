# C2 — AI Filter Layer

Part of Directive 001: Expand `mnq-robot` to Multi-Asset AI-Filtered Engine (MNQ + Gold).

## Scope
- New module `src/ai_filter/regime.py`.
- Connects to DeepSeek or OpenAI API (keys from `.env` only).
- Function: `get_market_regime(asset, current_price, vwap, recent_news_summary)`.
- Output: strict JSON `{"regime": "trending|ranging|volatile", "bias": "long|short|neutral", "risk_level": "low|high"}`.
- **CRITICAL:** This module only returns data. It has NO execution permissions. No order, send, or execute functions exist here.

## Dependencies
- C1 (Foundation): reads asset names and API configuration from `config.yaml` / `.env`.

## Core Functions

### 1. API wrapper (`regime.py`)
- Builds request to DeepSeek or OpenAI chat/completions endpoint.
- API key loaded exclusively from `.env` (never hardcoded).
- Graceful degradation: if API keys are missing or the call fails, returns a safe default regime rather than crashing.

### 2. `get_market_regime(asset, current_price, vwap, recent_news_summary)`
- Inputs: asset symbol, current price, VWAP, recent news summary string.
- Sends structured prompt to the LLM.
- Parses and validates the strict JSON schema.
- Rejects/fallbacks on malformed responses.

## Contract
- Returns only `{"regime", "bias", "risk_level"}`.
- Zero IBKR/execution imports or calls inside this module.
- Raises on invalid schema; never touches order flow.

## Definition of Done
- `src/ai_filter/regime.py` imports without API keys present (graceful degradation).
- Returns valid strict JSON for a mocked LLM response.
- No execution/send/order functions exist in the module.
- Data flow: C1 config → C2 AI filter → C4 strategy (gate input).
