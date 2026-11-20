# Research Findings and Strategy Decision Log

This is the durable record of what the analysis found, why it matters, and whether the evidence changed the investment thesis.

## Finding template

### FIND-YYYYMMDD-###
- **Title:**
- **Recorded at (local and UTC):**
- **Instrument / market scope:**
- **Question investigated:**
- **Finding type:** FACT / ESTIMATE / HYPOTHESIS / MODEL OUTPUT / UNKNOWN
- **Executive finding:**
- **Evidence and source references:**
- **Data period / freshness:**
- **Method:** calculations, indicators, model version and parameters
- **Results:** numerical results with units
- **Limitations / confounders:**
- **Alternative explanations:**
- **Confidence:** low / medium / high, with rationale
- **Portfolio relevance:**
- **Scenario impact:** bullish / base / bearish / neutral
- **Invalidation / next test:**
- **Decision status:** research only / monitor / proposed / human-approved / rejected
- **Reviewer and review date:**

## Strategy experiment template

- **Experiment ID:**
- **Strategy family:**
- **Hypothesis:**
- **Universe and instrument identifiers:**
- **Data source / sample period:**
- **Feature definitions:**
- **Entry / exit / sizing rules:**
- **Benchmark:**
- **Transaction cost model:** commissions, taxes, exchange fees, spread and slippage
- **Train / validation / test split:** chronological; no leakage
- **Metrics:** net return, drawdown, volatility, Sharpe/Sortino, hit rate, payoff, turnover and benchmark-relative performance
- **Regime robustness:**
- **Sensitivity / parameter stability:**
- **Known biases and limitations:**
- **Result:** pass / inconclusive / fail
- **Human review:**
- **Next action:**

## Findings

### FIND-20261120-001
- **Title:** BSM is a no-arbitrage PDE derived from delta hedging
- **Recorded at (local and UTC):** 2026-11-20, 2026-11-20T00:00:00Z
- **Instrument / market scope:** Options, derivatives markets (global)
- **Question investigated:** Is the Black-Scholes-Merton equation a valid no-arbitrage pricing relationship?
- **Finding type:** FACT
- **Executive finding:** BSM is a mathematically valid no-arbitrage PDE derived from dynamic replication (delta hedging) under geometric Brownian motion assumptions. It is not a return forecast.
- **Evidence and source references:** Black & Scholes (1973), DOI 10.1086/260062; Merton (1973), DOI 10.2307/3003143; docs/QUANT_TRADING_FULL_SWING_RESEARCH.md §3.2, §19 (claim #1)
- **Data period / freshness:** N/A (theoretical derivation, historical publication)
- **Method:** Mathematical derivation review; replication argument analysis
- **Results:** PDE: ∂V/∂t + ½σ²S²∂²V/∂S² + rS∂V/∂S − rV = 0
- **Limitations / confounders:** Assumes constant volatility, continuous hedging, frictionless markets — all violated in practice
- **Alternative explanations:** None for the no-arbitrage relationship itself
- **Confidence:** high — mathematically proven under stated assumptions
- **Portfolio relevance:** Provides relative-pricing benchmark and Greeks language for options risk management
- **Scenario impact:** neutral — benchmark, not directional signal
- **Invalidation / next test:** Empirical fit degrades during jumps, volatility clustering, and liquidity crises (see FIND-20261120-002 to -004)
- **Decision status:** research only
- **Reviewer and review date:** AI-powered architecture review, 2026-11-20

### FIND-20261120-002
- **Title:** Real asset returns have fat tails
- **Recorded at (local and UTC):** 2026-11-20, 2026-11-20T00:00:00Z
- **Instrument / market scope:** Equities, indices, FX, commodities
- **Question investigated:** Do financial returns follow the normal/lognormal distribution assumed by BSM?
- **Finding type:** FACT
- **Executive finding:** Empirical evidence consistently shows heavier tails than normal distribution. Tail index typically 2–5, excluding both normal and infinite-variance stable distributions.
- **Evidence and source references:** Mandelbrot (1963), DOI 10.1086/294632; Fama (1965), DOI 10.1086/294743; Cont (2001), DOI 10.1080/713665670; docs/QUANT_TRADING_FULL_SWING_RESEARCH.md §4.1, §19 (claim #3)
- **Data period / freshness:** Multiple decades across asset classes
- **Method:** Literature review of empirical distribution studies
- **Results:** 5-sigma+ moves occur far more often than normal model predicts
- **Limitations / confounders:** Exact tail index is sample-dependent; time-varying
- **Alternative explanations:** Stochastic volatility and jump processes can generate fat tails
- **Confidence:** high — robust across decades and asset classes
- **Portfolio relevance:** Variance-based risk measures understate tail losses; CVaR/stress testing required
- **Scenario impact:** bearish — tail risk is underpriced by normal models
- **Invalidation / next test:** Monitor kurtosis and tail index per instrument; stress-test portfolios
- **Decision status:** research only
- **Reviewer and review date:** AI-powered architecture review, 2026-11-20

### FIND-20261120-003
- **Title:** Volatility clusters and is time-varying
- **Recorded at (local and UTC):** 2026-11-20, 2026-11-20T00:00:00Z
- **Instrument / market scope:** Equities, indices, FX
- **Question investigated:** Is volatility constant (BSM assumption) or time-varying?
- **Finding type:** FACT
- **Executive finding:** Volatility exhibits strong positive autocorrelation (clustering). GARCH(1,1) persistence α+β often close to 1.
- **Evidence and source references:** Engle (1982), DOI 10.2307/1912773; Bollerslev (1986), DOI 10.1016/0304-4076(86)90063-1; Cont (2001), DOI 10.1080/713665670; docs/QUANT_TRADING_FULL_SWING_RESEARCH.md §4.2, §19 (claim #4)
- **Data period / freshness:** Decades of high-frequency return data
- **Method:** ARCH/GARCH model literature review
- **Results:** Volatility shocks decay slowly; constant-vol assumption is invalid
- **Limitations / confounders:** GARCH is one model among many; not complete description
- **Alternative explanations:** Stochastic volatility models (Heston), rough volatility
- **Confidence:** high — robust empirical regularity
- **Portfolio relevance:** Position sizing must adapt to volatility regimes; backtests assuming constant vol misstate drawdowns
- **Scenario impact:** neutral — risk management implication
- **Invalidation / next test:** Test GARCH vs stochastic-vol fit per instrument; regime-aware position sizing
- **Decision status:** research only
- **Reviewer and review date:** AI-powered architecture review, 2026-11-20

### FIND-20261120-004
- **Title:** Jumps invalidate continuous replication
- **Recorded at (local and UTC):** 2026-11-20, 2026-11-20T00:00:00Z
- **Instrument / market scope:** Equities, indices, options
- **Question investigated:** Do price jumps break BSM's continuous-time replication argument?
- **Finding type:** FACT
- **Executive finding:** Discontinuous price jumps prevent continuous delta hedging from replicating option payoffs; hedge portfolio bears jump risk.
- **Evidence and source references:** Merton (1976), DOI 10.1016/0304-405X(76)90022-2; docs/QUANT_TRADING_FULL_SWING_RESEARCH.md §4.3, §19 (claim #5)
- **Data period / freshness:** Theoretical + empirical (crash events)
- **Method:** Theoretical derivation + jump-diffusion model review
- **Results:** Jump-diffusion model adds Poisson component to GBM
- **Limitations / confounders:** Jump parameters are latent and hard to estimate
- **Alternative explanations:** Stochastic-local volatility models also capture jump-like behavior
- **Confidence:** high — theoretically proven, empirically observed during crashes
- **Portfolio relevance:** Gap risk and jump risk must be modeled separately from diffusion risk
- **Scenario impact:** bearish — crash protection (OTM puts) commands premium
- **Invalidation / next test:** Monitor jump arrival and size per instrument; stress-test gap scenarios
- **Decision status:** research only
- **Reviewer and review date:** AI-powered architecture review, 2026-11-20

### FIND-20261120-005
- **Title:** Implied-volatility smile/skew persists post-1987
- **Recorded at (local and UTC):** 2026-11-20, 2026-11-20T00:00:00Z
- **Instrument / market scope:** Equity index options
- **Question investigated:** Is implied volatility flat across strikes (BSM implication) or skewed?
- **Finding type:** FACT
- **Executive finding:** OTM puts trade at higher implied volatility than ATM options; negative skew persists after 1987 crash. Market prices crash fear and jump risk.
- **Evidence and source references:** Rubinstein (1994), DOI 10.1111/j.1540-6261.1994.tb00079.x; Jackwerth & Rubinstein (1996), DOI 10.1111/j.1540-6261.1996.tb05219.x; Bates (2000), DOI 10.1016/S0304-4076(99)00021-4; docs/QUANT_TRADING_FULL_SWING_RESEARCH.md §4.4, §19 (claim #6)
- **Data period / freshness:** Post-1987 to present
- **Method:** Empirical options market data review
- **Results:** Smile/skew is a direct rejection of constant-volatility lognormal model
- **Limitations / confounders:** Smile is quoting convention as much as distribution forecast
- **Alternative explanations:** Stochastic volatility, jump risk, supply-demand imbalances
- **Confidence:** high — persistent, observable market phenomenon
- **Portfolio relevance:** Options trading must model the full surface, not single volatility number
- **Scenario impact:** bearish — downside protection is structurally expensive
- **Invalidation / next test:** Track smile/skew evolution per underlying; surface calibration accuracy
- **Decision status:** research only
- **Reviewer and review date:** AI-powered architecture review, 2026-11-20

### FIND-20261120-006
- **Title:** BSM is a relative-pricing model, not a return forecast
- **Recorded at (local and UTC):** 2026-11-20, 2026-11-20T00:00:00Z
- **Instrument / market scope:** Options markets
- **Question investigated:** Does BSM forecast stock expected returns?
- **Finding type:** ANALYSIS
- **Executive finding:** Expected stock drift μ cancels out of the replication argument. BSM prices options relative to underlying, risk-free rate, time, and volatility only.
- **Evidence and source references:** Derivation in docs/QUANT_TRADING_FULL_SWING_RESEARCH.md §3.2; market practice; §19 (claim #8)
- **Data period / freshness:** N/A (theoretical result)
- **Method:** Replication argument analysis
- **results:** Drift term disappears from BSM PDE
- **Limitations / confounders:** None for the theoretical result
- **Alternative explanations:** None — derivation is unambiguous
- **Confidence:** high — mathematically certain
- **Portfolio relevance:** Do not use BSM to forecast directional returns; use for relative pricing and Greeks only
- **Scenario impact:** neutral — modeling posture
- **Invalidation / next test:** N/A
- **Decision status:** research only
- **Reviewer and review date:** AI-powered architecture review, 2026-11-20

### FIND-20261120-007
- **Title:** Medallion-like returns are not publicly replicable
- **Recorded at (local and UTC):** 2026-11-20, 2026-11-20T00:00:00Z
- **Instrument / market scope:** Quantitative hedge funds (Renaissance Technologies Medallion)
- **Question investigated:** Can retail investors replicate Medallion's reported returns?
- **Finding type:** HYPOTHESIS / UNKNOWN
- **Executive finding:** Medallion reported ~66.1% gross avg annual return (1988–2018) per Cornell reconstruction, but fund is closed to outsiders, capacity-constrained, opaque, and not a BSM strategy (stat arb, not options pricing). Returns are not audited public data.
- **Evidence and source references:** Cornell (2020), Zuckerman (2019); docs/QUANT_TRADING_FULL_SWING_RESEARCH.md §9, §19 (claims #16–18)
- **Data period / freshness:** 1988–2018 reconstruction
- **Method:** Academic reconstruction of journalist's appendix data
- **Results:** ~$100 → ~$398.7M compound gross (63.3% CAGR); no negative gross year
- **Limitations / confounders:** Not audited; reconstruction from appendix; averages across annual observations
- **Alternative explanations:** None that explain the magnitude without proprietary data/execution
- **Confidence:** medium–high — strong access and capacity barriers
- **Portfolio relevance:** Do not target Medallion-like returns; focus on process discipline and cost control instead
- **Scenario impact:** neutral — expectation setting
- **Invalidation / next test:** Monitor for any public audited vehicle with comparable track record
- **Decision status:** research only
- **Reviewer and review date:** AI-powered architecture review, 2026-11-20

### FIND-20261120-008
- **Title:** LTCM leverage and liquidity mismatch were central to collapse
- **Recorded at (local and UTC):** 2026-11-20, 2026-11-20T00:00:00Z
- **Instrument / market scope:** Fixed income, convergence trades, derivatives
- **Question investigated:** What caused LTCM's 1998 collapse?
- **Finding type:** FACT
- **Executive finding:** LTCM ran ~$5B equity, >$125B borrowed, >$1T notional derivatives (25–30:1 leverage). Russian default + flight to liquidity caused spreads to widen, correlations to one, margin calls, insolvency. Fed-organized $3.6B consortium recapitalization.
- **Evidence and source references:** Edwards (1999), DOI 10.1257/jep.13.2.189; Federal Reserve History; Lowenstein (2000), ISBN 978-0-375-75825-6; docs/QUANT_TRADING_FULL_SWING_RESEARCH.md §10, §19 (claim #19)
- **Data period / freshness:** 1994–1998
- **Method:** Historical case study review
- **Results:** Leverage converts model errors into insolvency; correlation assumptions fail in crises
- **Limitations / confounders:** Causal interpretation of model blame is ANALYSIS
- **Alternative explanations:** Rogue trader, fraud — not supported by evidence
- **Confidence:** high — well-documented historical event
- **Portfolio relevance:** Leverage caps, drawdown controls, kill criteria, liquidity stress testing are mandatory
- **Scenario impact:** bearish — leverage + correlation breakdown = systemic risk
- **Invalidation / next test:** Apply LTCM lessons to any leverage decision; stress-test correlation regime changes
- **Decision status:** research only
- **Reviewer and review date:** AI-powered architecture review, 2026-11-20

## Decision journal

A recommendation is not a trade. Record proposed, approved, rejected and executed states separately. An executed trade requires independent broker/account evidence and must not be inferred from a model output or chat statement.
