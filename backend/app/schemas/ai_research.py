"""Schemas for AI research endpoints."""

from pydantic import BaseModel, Field
from typing import Optional, List, Dict, Any
from datetime import datetime


class ResearchMemoRequest(BaseModel):
    instrument_id: str = Field(..., description="Instrument identifier")
    market_context: str = Field(..., description="Current market observations")
    research_question: str = Field(..., description="Question to investigate")
    evidence_classification: str = Field(
        default="HYPOTHESIS",
        pattern="^(FACT|ESTIMATE|HYPOTHESIS|MODEL_OUTPUT|UNKNOWN)$",
        description="Evidence label for the output",
    )


class ResearchMemoResponse(BaseModel):
    memo_id: str
    instrument_id: str
    title: str
    executive_summary: str
    bull_case: str
    base_case: str
    bear_case: str
    invalidation_conditions: List[str]
    confidence: str = Field(..., pattern="^(low|medium|high)$")
    evidence_classification: str
    sources: List[str]
    generated_at: datetime
    model_used: str
    research_only: bool = True
    not_financial_advice: bool = True


class SignalRequest(BaseModel):
    instrument_id: str
    strategy_family: str = Field(
        ..., description="trend|mean_reversion|breakout|fundamental|event_driven|risk_management"
    )
    lookback_period: str = Field(default="90d")
    risk_budget: Optional[float] = Field(default=None, description="Max position risk %")


class SignalResponse(BaseModel):
    signal_id: str
    instrument_id: str
    side: Optional[str] = Field(None, pattern="^(BUY|SELL|HOLD)$")
    strength: float = Field(..., ge=0.0, le=1.0, description="Signal confidence 0-1")
    reasoning: str
    evidence_classification: str
    suggested_plan: Optional[dict] = None
    human_review_required: bool = True
    generated_at: datetime


class SummarizeRequest(BaseModel):
    texts: List[str] = Field(..., min_length=1, description="Texts to summarize")
    topic: str = Field(..., description="Topic area for classification")


class SummarizeResponse(BaseModel):
    summary_id: str
    summary: str
    key_points: List[str]
    evidence_classification: str
    generated_at: datetime


class ChatRequest(BaseModel):
    message: str = Field(..., min_length=1)
    conversation_id: Optional[str] = None
    context: Optional[dict] = None


class ChatResponse(BaseModel):
    response: str
    conversation_id: str
    evidence_classification: str
    sources: List[str] = []
