"""Transaction cost model for baseline strategies.

Includes commissions, exchange fees, spread, slippage, and taxes.
All amounts as decimal strings per repository convention.
"""

from dataclasses import dataclass
from typing import Optional


@dataclass
class CostModel:
    """Transaction cost parameters per trade side."""

    commission_rate: float  # % of notional (e.g., 0.001 = 0.1%)
    exchange_fee_rate: float  # % of notional
    spread_bps: float  # basis points (half-spread)
    slippage_bps: float  # basis points
    tax_rate: float  # % of notional (sell side only typically)
    min_commission: float  # minimum commission per trade in currency

    def total_cost_rate(self, is_buy: bool = True) -> float:
        """Total cost rate as fraction of notional."""
        rate = self.commission_rate + self.exchange_fee_rate + self.spread_bps / 10000 + self.slippage_bps / 10000
        if not is_buy:
            rate += self.tax_rate
        return rate

    def compute_costs(
        self,
        notional: float,
        is_buy: bool = True,
        quantity: Optional[int] = None,
    ) -> dict:
        """Compute all cost components.

        Returns dict with commission, exchange_fee, spread, slippage, tax, total.
        All values as float amounts (not decimal strings).
        """
        commission = max(self.commission_rate * notional, self.min_commission)
        exchange_fee = self.exchange_fee_rate * notional
        spread = (self.spread_bps / 10000) * notional
        slippage = (self.slippage_bps / 10000) * notional
        tax = self.tax_rate * notional if not is_buy else 0.0
        total = commission + exchange_fee + spread + slippage + tax

        return {
            "commission": round(commission, 2),
            "exchange_fee": round(exchange_fee, 2),
            "spread": round(spread, 2),
            "slippage": round(slippage, 2),
            "tax": round(tax, 2),
            "total": round(total, 2),
            "total_rate": round(self.total_cost_rate(is_buy), 6),
        }


# Default cost model: Philippine equities (PSE) illustrative
DEFAULT_COST_MODEL = CostModel(
    commission_rate=0.0025,      # 0.25% commission
    exchange_fee_rate=0.0001,    # 0.01% exchange fee
    spread_bps=5.0,              # 5 bps half-spread
    slippage_bps=2.0,            # 2 bps slippage
    tax_rate=0.006,              # 0.6% stock transaction tax (sell side)
    min_commission=20.0,         # PHP 20 minimum commission
)
