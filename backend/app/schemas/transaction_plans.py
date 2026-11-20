"""Schemas for transaction plan submission (human-review queue)."""

from pydantic import BaseModel, Field, conint, constr
from datetime import datetime
from typing import Optional


class TransactionPlanCreate(BaseModel):
    instrument_id: constr(strip_whitespace=True, min_length=1) = Field(
        ..., description="Verified instrument identifier"
    )
    side: str = Field(..., pattern="^(BUY|SELL)$", description="BUY or SELL")
    quantity: conint(ge=1) = Field(..., description="Order quantity in shares/units")
    price: str = Field(
        ..., pattern=r"^[0-9]+(\.[0-9]+)?$", description="Limit price as decimal string"
    )
    timestamp: datetime = Field(..., description="Plan submission time (UTC)")
    thesis: str = Field(
        ..., min_length=1, description="Investment thesis and reasoning"
    )
    evidence: str = Field(
        ..., min_length=1, description="Evidence references and source URLs"
    )
    human_approval: bool = Field(
        ..., description="Must be true — human has reviewed and approved"
    )

    class Config:
        json_schema_extra = {
            "example": {
                "instrument_id": "INSTR-AAPL-001",
                "side": "BUY",
                "quantity": 100,
                "price": "185.50",
                "timestamp": "2026-11-20T14:30:00Z",
                "thesis": "Mean reversion after oversold RSI condition",
                "evidence": "Market bar data from exchange, RSI(14) < 30",
                "human_approval": True,
            }
        }


class TransactionPlanResponse(BaseModel):
    plan_id: str
    status: str = Field(
        ..., pattern="^(submitted|under_review|approved|rejected)$"
    )
    submitted_at: datetime
    research_only: bool = True
    autonomous_execution: bool = False

    class Config:
        json_schema_extra = {
            "example": {
                "plan_id": "PLAN-20261120-001",
                "status": "submitted",
                "submitted_at": "2026-11-20T14:30:00Z",
                "research_only": True,
                "autonomous_execution": False,
            }
        }
