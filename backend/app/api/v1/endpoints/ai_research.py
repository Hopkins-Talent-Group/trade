"""AI research endpoints — research assistance, not trade advice.

All AI outputs carry evidence classification labels per docs/AI_QUANT_STRATEGY.md.
Human review is required before any action is taken on AI output.
"""

import uuid
from datetime import datetime, timezone
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
    """Generate a signal suggestion. Human review required before any action."""
    signal_id = f"SIG-{uuid.uuid4().hex[:8].upper()}"
    now = datetime.now(timezone.utc)

    # TODO: integrate C++ engine bridge + LLM for signal generation
    signal = SignalResponse(
        signal_id=signal_id,
        instrument_id=request.instrument_id,
        side=None,
        strength=0.0,
        reasoning=(
            f"Signal generation for {request.instrument_id} "
            f"({request.strategy_family}) is a placeholder. "
            f"Human review required. {EVIDENCE_DISCLAIMER}"
        ),
        evidence_classification="HYPOTHESIS",
        suggested_plan=None,
        human_review_required=True,
        generated_at=now,
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
