"""AI research endpoints — research assistance, not trade advice.

All AI outputs carry evidence classification labels per docs/AI_QUANT_STRATEGY.md.
Human review is required before any action is taken on AI output.
"""

import uuid
from datetime import datetime, timezone
from typing import Optional
from fastapi import APIRouter, Depends, HTTPException, status

from app.core.security import require_researcher
from app.schemas.ai_research import (
    ResearchMemoRequest,
    ResearchMemoResponse,
    SignalRequest,
    SignalResponse,
    SummarizeRequest,
    SummarizeResponse,
    ChatRequest,
    ChatResponse,
)
from app.strategies.baseline import (
    TrendFollowingSignal,
    MeanReversionSignal,
    compute_all_signals,
)
from app.strategies.cost_model import DEFAULT_COST_MODEL

router = APIRouter()

# In-memory stores for development. Replace with database persistence.
_memo_store: dict = {}
_signal_store: dict = {}
_summary_store: dict = {}
_chat_store: dict = {}

EVIDENCE_DISCLAIMER = (
    "This is AI-generated research assistance, not individualized financial advice. "
    "All outputs require human review before any action."
)


@router.post(
    "/ai/research",
    response_model=ResearchMemoResponse,
    summary="Generate AI-assisted research memo draft",
)
async def generate_research_memo(
    request: ResearchMemoRequest,
    user: dict = Depends(require_researcher),
):
    """Draft a research memo using AI. Output is a hypothesis, not a trade signal."""
    memo_id = f"MEMO-{uuid.uuid4().hex[:8].upper()}"
    now = datetime.now(timezone.utc)

    # TODO: integrate LLM via LangChain (OpenAI/Anthropic)
    memo = ResearchMemoResponse(
        memo_id=memo_id,
        instrument_id=request.instrument_id,
        title=f"AI Research Draft: {request.instrument_id}",
        executive_summary=(
            f"AI-generated draft for {request.instrument_id}. "
            f"Question: {request.research_question}. {EVIDENCE_DISCLAIMER}"
        ),
        bull_case="AI-generated bull case placeholder.",
        base_case="AI-generated base case placeholder.",
        bear_case="AI-generated bear case placeholder.",
        invalidation_conditions=[
            "Price invalidates thesis",
            "Macro regime change",
            "New contradictory evidence",
        ],
        confidence="medium",
        evidence_classification=request.evidence_classification,
        sources=[],
        generated_at=now,
        model_used="placeholder-llm",
    )
    _memo_store[memo_id] = memo.model_dump()
    return memo


@router.post(
    "/ai/signal",
    response_model=SignalResponse,
    summary="Generate AI-assisted trading signal (human review required)",
)
async def generate_signal(
    request: SignalRequest,
    user: dict = Depends(require_researcher),
):
    """Generate a signal suggestion using baseline strategies.

    Supports trend and mean_reversion strategy families.
    Human review required before any action.
    """
    signal_id = f"SIG-{uuid.uuid4().hex[:8].upper()}"
    now = datetime.now(timezone.utc)

    prices = request.prices or []
    timestamps = request.timestamps or []

    if not prices:
        signal = SignalResponse(
            signal_id=signal_id,
            instrument_id=request.instrument_id,
            side=None,
            strength=0.0,
            reasoning=(
                f"No price data provided for {request.instrument_id}. "
                f"Cannot compute {request.strategy_family} signal. "
                f"Human review required. {EVIDENCE_DISCLAIMER}"
            ),
            evidence_classification="HYPOTHESIS",
            suggested_plan=None,
            human_review_required=True,
            generated_at=now,
            strategy_family=request.strategy_family,
        )
        _signal_store[signal_id] = signal.model_dump()
        return signal

    strategy_family = request.strategy_family.lower()

    if strategy_family == "trend":
        signal_obj = TrendFollowingSignal().compute(
            request.instrument_id, prices, timestamps, DEFAULT_COST_MODEL
        )
    elif strategy_family == "mean_reversion":
        signal_obj = MeanReversionSignal().compute(
            request.instrument_id, prices, timestamps, DEFAULT_COST_MODEL
        )
    else:
        # Unsupported family - return HOLD
        signal_obj = type(
            "DummySignal",
            (),
            {
                "instrument_id": request.instrument_id,
                "strategy_family": strategy_family,
                "side": "HOLD",
                "strength": 0.0,
                "price": prices[-1],
                "timestamp": now.isoformat(),
                "cost_model": DEFAULT_COST_MODEL,
                "evidence_classification": "HYPOTHESIS",
                "reasoning": (
                    f"Strategy family '{request.strategy_family}' not implemented. "
                    f"Supported: trend, mean_reversion. {EVIDENCE_DISCLAIMER}"
                ),
                "suggested_plan": None,
                "to_dict": lambda self: {
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
                },
            },
        )(
            request.instrument_id,
            strategy_family,
            "HOLD",
            0.0,
            prices[-1],
            now.isoformat(),
            DEFAULT_COST_MODEL,
        )

    signal = SignalResponse(
        signal_id=signal_id,
        instrument_id=signal_obj.instrument_id,
        side=signal_obj.side,
        strength=signal_obj.strength,
        reasoning=signal_obj.reasoning,
        evidence_classification=signal_obj.evidence_classification,
        suggested_plan=signal_obj.suggested_plan,
        human_review_required=True,
        generated_at=now,
        strategy_family=signal_obj.strategy_family,
    )
    _signal_store[signal_id] = signal.model_dump()
    return signal


@router.post(
    "/ai/summarize",
    response_model=SummarizeResponse,
    summary="Summarize texts into research findings",
)
async def summarize_texts(
    request: SummarizeRequest,
    user: dict = Depends(require_researcher),
):
    """Summarize source texts into key research points."""
    summary_id = f"SUM-{uuid.uuid4().hex[:8].upper()}"
    now = datetime.now(timezone.utc)

    # TODO: integrate LLM summarization
    summary = SummarizeResponse(
        summary_id=summary_id,
        summary=f"AI summary of {len(request.texts)} text(s) on '{request.topic}'. {EVIDENCE_DISCLAIMER}",
        key_points=["Placeholder key point 1", "Placeholder key point 2"],
        evidence_classification="HYPOTHESIS",
        generated_at=now,
    )
    _summary_store[summary_id] = summary.model_dump()
    return summary


@router.post(
    "/ai/chat",
    response_model=ChatResponse,
    summary="Conversational research assistant",
)
async def ai_chat(
    request: ChatRequest,
    user: dict = Depends(require_researcher),
):
    """Chat with AI research assistant. Outputs require human verification."""
    conversation_id = request.conversation_id or f"CHAT-{uuid.uuid4().hex[:8].upper()}"
    now = datetime.now(timezone.utc)

    # TODO: integrate LLM chat with conversation memory
    response = ChatResponse(
        response=(
            f"I'm a research assistant. You asked: '{request.message}'. "
            f"This is AI-generated output requiring human verification. "
            f"{EVIDENCE_DISCLAIMER}"
        ),
        conversation_id=conversation_id,
        evidence_classification="HYPOTHESIS",
        sources=[],
    )
    _chat_store[conversation_id] = {
        "messages": [_chat_store.get(conversation_id, {}).get("messages", []), response.model_dump()]
    }
    return response
