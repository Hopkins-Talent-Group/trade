"""Transaction ledger endpoint (read-only)."""

from fastapi import APIRouter, Query, Depends
from typing import Optional
from datetime import datetime

from app.core.security import require_researcher

router = APIRouter()


@router.get("/transactions", summary="Retrieve transaction ledger records")
async def get_transactions(
    from_date: Optional[datetime] = Query(None, alias="from"),
    to_date: Optional[datetime] = Query(None, alias="to"),
    instrument_id: Optional[str] = Query(None),
    status: Optional[str] = Query(
        None, pattern="^(filled|partial|cancelled|expired)$"
    ),
    user: dict = Depends(require_researcher),
):
    """Return transaction ledger entries. Read-only research access."""
    # TODO: connect to database / C++ engine position store
    return {
        "schema_version": "1.0.0",
        "data": [],
        "filters": {
            "from": from_date,
            "to": to_date,
            "instrument_id": instrument_id,
            "status": status,
        },
        "research_only": True,
    }
