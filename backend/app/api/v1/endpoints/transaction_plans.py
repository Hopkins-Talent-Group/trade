"""Transaction plan submission endpoint (human-review queue).

Research-only: submits a plan for human review. Does NOT execute orders.
"""

import uuid
from datetime import datetime, timezone
from fastapi import APIRouter, Depends, HTTPException, status

from app.core.security import require_researcher
from app.schemas.transaction_plans import (
    TransactionPlanCreate,
    TransactionPlanResponse,
)

router = APIRouter()

# In-memory plan store for development. Replace with database persistence.
_plan_store: dict = {}


@router.post(
    "/transaction-plans",
    response_model=TransactionPlanResponse,
    status_code=status.HTTP_201_CREATED,
    summary="Submit a transaction plan for human review",
)
async def submit_transaction_plan(
    plan: TransactionPlanCreate,
    user: dict = Depends(require_researcher),
):
    """Submit a transaction plan for human review.

    - Requires human_approval=True
    - Does NOT execute any order
    - Plan enters the review queue for a human investment committee
    """
    if not plan.human_approval:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="human_approval must be true",
        )

    plan_id = f"PLAN-{datetime.now(timezone.utc).strftime('%Y%m%d')}-{uuid.uuid4().hex[:4].upper()}"
    now = datetime.now(timezone.utc)

    record = {
        "plan_id": plan_id,
        "status": "submitted",
        "submitted_at": now,
        "plan": plan.model_dump(),
        "submitted_by": user.get("sub", "unknown"),
        "research_only": True,
        "autonomous_execution": False,
    }
    _plan_store[plan_id] = record

    return TransactionPlanResponse(
        plan_id=plan_id,
        status="submitted",
        submitted_at=now,
    )
