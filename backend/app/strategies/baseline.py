"""Baseline strategies: trend following and mean reversion.

Signals are research artifacts. Human review required before any action.
All outputs carry evidence classification labels.
"""

from dataclasses import dataclass
from typing import Optional, List
from datetime import datetime, timezone

from app.strategies.cost_model import CostModel, DEFAULT_COST_MODEL


@dataclass
class Signal:
    """Trading signal output."""

    instrument_id: str
    strategy_family: str  # "trend" | "mean_reversion" | "breakout"
    side: str  # "BUY" | "SELL" | "HOLD"
    strength: float  # 0.0 to 1.0
    price: float
    timestamp: str
    cost_model: CostModel
    evidence_classification: str = "HYPOTHESIS"
    reasoning: str = ""
    suggested_plan: Optional[dict] = None

    def to_dict(self) -> dict:
        return {
            "instrument_id": self.instrument_id,
            "strategy_family": self.strategy_family,
            "side": self.side,
            "strength": self.strength,
            "price": self.price,
            "timestamp": self.timestamp,
            "evidence_classification": self.evidence_classification,
            "reasoning": self.reasoning,
            "suggested_plan": self.suggested_plan,
            "human_review_required": True,
            "research_only": True,
        }


class TrendFollowingSignal:
    """Simple trend following signal based on moving average crossover.

    Uses fast MA vs slow MA crossover. Bullish when fast > slow.
    Bearish when fast < slow.
    """

    def __init__(self, fast_window: int = 10, slow_window: int = 30):
        self.fast_window = fast_window
        self.slow_window = slow_window

    def compute(
        self,
        instrument_id: str,
        prices: List[float],
        timestamps: Optional[List[str]] = None,
        cost_model: CostModel = DEFAULT_COST_MODEL,
    ) -> Signal:
        """Compute trend signal from price series.

        Returns BUY if fast MA crossed above slow MA (bullish crossover).
        Returns SELL if fast MA crossed below slow MA (bearish crossover).
        Returns HOLD otherwise.
        """
        if len(prices) < self.slow_window + 1:
            return Signal(
                instrument_id=instrument_id,
                strategy_family="trend",
                side="HOLD",
                strength=0.0,
                price=prices[-1] if prices else 0.0,
                timestamp=timestamps[-1] if timestamps else datetime.now(timezone.utc).isoformat(),
                cost_model=cost_model,
                evidence_classification="HYPOTHESIS",
                reasoning="Insufficient data for trend signal (need >= {} bars)".format(
                    self.slow_window + 1
                ),
            )

        fast_ma = sum(prices[-self.fast_window:]) / self.fast_window
        slow_ma = sum(prices[-self.slow_window:]) / self.slow_window

        prev_fast_ma = sum(prices[-(self.fast_window + 1) : -1]) / self.fast_window
        prev_slow_ma = sum(prices[-(self.slow_window + 1) : -1]) / self.slow_window

        current_price = prices[-1]
        timestamp = timestamps[-1] if timestamps else datetime.now(timezone.utc).isoformat()

        # Bullish crossover: fast crossed above slow
        if prev_fast_ma <= prev_slow_ma and fast_ma > slow_ma:
            strength = min(0.9, abs(fast_ma - slow_ma) / slow_ma * 10)
            side = "BUY"
            reasoning = (
                f"Trend bullish crossover: fast MA ({fast_ma:.2f}) > slow MA ({slow_ma:.2f}). "
                f"Previous: fast ({prev_fast_ma:.2f}) <= slow ({prev_slow_ma:.2f})."
            )
        # Bearish crossover: fast crossed below slow
        elif prev_fast_ma >= prev_slow_ma and fast_ma < slow_ma:
            strength = min(0.9, abs(fast_ma - slow_ma) / slow_ma * 10)
            side = "SELL"
            reasoning = (
                f"Trend bearish crossover: fast MA ({fast_ma:.2f}) < slow MA ({slow_ma:.2f}). "
                f"Previous: fast ({prev_fast_ma:.2f}) >= slow ({prev_slow_ma:.2f})."
            )
        else:
            # No crossover - hold but show trend direction
            if fast_ma > slow_ma:
                side = "HOLD"
                strength = min(0.5, abs(fast_ma - slow_ma) / slow_ma * 5)
                reasoning = (
                    f"Trend bullish but no crossover: fast MA ({fast_ma:.2f}) > slow MA ({slow_ma:.2f}). "
                    f"Awaiting confirmation."
                )
            else:
                side = "HOLD"
                strength = min(0.5, abs(fast_ma - slow_ma) / slow_ma * 5)
                reasoning = (
                    f"Trend bearish but no crossover: fast MA ({fast_ma:.2f}) < slow MA ({slow_ma:.2f}). "
                    f"Awaiting confirmation."
                )

        costs = cost_model.compute_costs(current_price, is_buy=(side == "BUY"))

        suggested_plan = None
        if side in ("BUY", "SELL"):
            suggested_plan = {
                "instrument_id": instrument_id,
                "side": side,
                "quantity": 100,  # illustrative; human must adjust
                "price": f"{current_price:.2f}",
                "timestamp": timestamp,
                "thesis": reasoning,
                "evidence": "Moving average crossover; cost model applied",
                "human_approval": True,
                "estimated_costs": costs,
            }

        return Signal(
            instrument_id=instrument_id,
            strategy_family="trend",
            side=side,
            strength=strength,
            price=current_price,
            timestamp=timestamp,
            cost_model=cost_model,
            evidence_classification="HYPOTHESIS",
            reasoning=reasoning,
            suggested_plan=suggested_plan,
        )


class MeanReversionSignal:
    """Mean reversion signal based on deviation from rolling mean (z-score).

    Uses z-score of price vs rolling mean. Extreme negative z-score = BUY (oversold).
    Extreme positive z-score = SELL (overbought).
    """

    def __init__(self, window: int = 20, entry_z: float = 2.0, exit_z: float = 0.5):
        self.window = window
        self.entry_z = entry_z
        self.exit_z = exit_z

    def compute(
        self,
        instrument_id: str,
        prices: List[float],
        timestamps: Optional[List[str]] = None,
        cost_model: CostModel = DEFAULT_COST_MODEL,
    ) -> Signal:
        """Compute mean reversion signal from price series.

        Returns BUY if z-score <= -entry_z (oversold, expect reversion up).
        Returns SELL if z-score >= entry_z (overbought, expect reversion down).
        Returns HOLD otherwise.
        """
        if len(prices) < self.window:
            return Signal(
                instrument_id=instrument_id,
                strategy_family="mean_reversion",
                side="HOLD",
                strength=0.0,
                price=prices[-1] if prices else 0.0,
                timestamp=timestamps[-1] if timestamps else datetime.now(timezone.utc).isoformat(),
                cost_model=cost_model,
                evidence_classification="HYPOTHESIS",
                reasoning=f"Insufficient data for mean reversion (need >= {self.window} bars)",
            )

        window_prices = prices[-self.window:]
        mean = sum(window_prices) / self.window
        variance = sum((p - mean) ** 2 for p in window_prices) / self.window
        std = variance ** 0.5

        if std == 0:
            z_score = 0.0
        else:
            z_score = (prices[-1] - mean) / std

        current_price = prices[-1]
        timestamp = timestamps[-1] if timestamps else datetime.now(timezone.utc).isoformat()

        if z_score <= -self.entry_z:
            side = "BUY"
            strength = min(0.9, abs(z_score) / self.entry_z * 0.5)
            reasoning = (
                f"Mean reversion oversold: z-score ({z_score:.2f}) <= -{self.entry_z}. "
                f"Price {current_price:.2f} below rolling mean {mean:.2f}. Expect reversion up."
            )
        elif z_score >= self.entry_z:
            side = "SELL"
            strength = min(0.9, abs(z_score) / self.entry_z * 0.5)
            reasoning = (
                f"Mean reversion overbought: z-score ({z_score:.2f}) >= {self.entry_z}. "
                f"Price {current_price:.2f} above rolling mean {mean:.2f}. Expect reversion down."
            )
        elif abs(z_score) <= self.exit_z:
            side = "HOLD"
            strength = 0.3
            reasoning = (
                f"Mean reversion neutral: z-score ({z_score:.2f}) within exit band "
                f"[-{self.exit_z}, {self.exit_z}]. No signal."
            )
        else:
            side = "HOLD"
            strength = 0.4
            reasoning = (
                f"Mean reversion fading: z-score ({z_score:.2f}) moving toward mean. "
                f"Awaiting extreme condition."
            )

        costs = cost_model.compute_costs(current_price, is_buy=(side == "BUY"))

        suggested_plan = None
        if side in ("BUY", "SELL"):
            suggested_plan = {
                "instrument_id": instrument_id,
                "side": side,
                "quantity": 100,  # illustrative; human must adjust
                "price": f"{current_price:.2f}",
                "timestamp": timestamp,
                "thesis": reasoning,
                "evidence": f"Z-score {z_score:.2f}; rolling mean {mean:.2f}; cost model applied",
                "human_approval": True,
                "estimated_costs": costs,
            }

        return Signal(
            instrument_id=instrument_id,
            strategy_family="mean_reversion",
            side=side,
            strength=strength,
            price=current_price,
            timestamp=timestamp,
            cost_model=cost_model,
            evidence_classification="HYPOTHESIS",
            reasoning=reasoning,
            suggested_plan=suggested_plan,
        )


def compute_all_signals(
    instrument_id: str,
    prices: List[float],
    timestamps: Optional[List[str]] = None,
    cost_model: CostModel = DEFAULT_COST_MODEL,
) -> dict:
    """Compute all baseline strategies for an instrument.

    Returns dict with trend and mean_reversion signals.
    """
    trend = TrendFollowingSignal()
    mean_rev = MeanReversionSignal()

    return {
        "instrument_id": instrument_id,
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "trend": trend.compute(instrument_id, prices, timestamps, cost_model).to_dict(),
        "mean_reversion": mean_rev.compute(instrument_id, prices, timestamps, cost_model).to_dict(),
        "research_only": True,
        "human_review_required": True,
        "evidence_classification": "HYPOTHESIS",
        "disclaimer": "AI-generated baseline strategy signals are research artifacts, not trade recommendations. Human approval required.",
    }
