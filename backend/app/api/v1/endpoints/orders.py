"""Order status endpoint (read-only, no execution)."""

from fastapi import APIRouter, Query, Depends
from typing import Optional

from app.core.security import require_researcher

router = APIRouter()


@router.get("/orders", summary="Retrieve order status records")
async def get_orders(
    instrument_id: Optional[str] = Query(None),
    status: Optional[str] = Query(
        None, pattern="^(pending|filled|partial|cancelled|expired)$"
    ),
    user: dict = Depends(require_researcher),
):
    """Return order records. Read-only — no order placement or modification."""
    # TODO: connect to database / broker read-only adapter
    return {
        "schema_version": "1.0.0",
        "data": [],
        "filters": {"instrument_id": instrument_id, "status": status},
        "research_only": True,
        "autonomous_execution": False,
    }
