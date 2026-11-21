"""Order status endpoint (read-only, no execution)."""

from fastapi import APIRouter, Query, Depends
from typing import Optional, List
from datetime import datetime, timezone

from app.core.security import require_researcher

router = APIRouter()

# In-memory order store for development. Replace with database persistence.
_order_store: dict = {}

# Seed with one illustrative order
_order_store["ORD-20261120-001"] = {
    "schema_version": "1.0.0",
    "order_id": "ORD-20261120-001",
    "instrument_id": "INSTR-PSE-MER-001",
    "symbol": "MER",
    "side": "BUY",
    "quantity": 500,
    "price": "315.00",
    "status": "pending",
    "filled_quantity": 0,
    "average_fill_price": None,
    "time_in_force": "DAY",
    "currency": "PHP",
    "evidence_status": "user_reported",
    "created_at": "2026-11-20T09:00:00+08:00",
    "updated_at": "2026-11-20T09:00:00+08:00",
    "source": {
        "provider": "user_reported",
        "record_url": None,
        "retrieved_at": "2026-11-20T00:00:00+08:00",
        "published_at": None,
        "provenance_status": "unverified",
        "raw_payload_id": None,
        "license_note": "Illustrative fixture only; not broker-verified.",
    },
    "quality": {
        "freshness": "current",
        "issues": ["User-reported; not independently verified against broker order book"],
        "confidence": 0.5,
    },
}


@router.get("/orders", summary="Retrieve order status records")
async def get_orders(
    instrument_id: Optional[str] = Query(None),
    status: Optional[str] = Query(
        None, pattern="^(pending|filled|partial|cancelled|expired)$"
    ),
    user: dict = Depends(require_researcher),
):
    """Return order records. Read-only — no order placement or modification."""
    results: List[dict] = []
    for order in _order_store.values():
        if instrument_id and order["instrument_id"] != instrument_id:
            continue
        if status and order["status"] != status:
            continue
        results.append(order)

    return {
        "schema_version": "1.0.0",
        "data": results,
        "count": len(results),
        "filters": {"instrument_id": instrument_id, "status": status},
        "research_only": True,
        "autonomous_execution": False,
        "evidence_note": "All orders are user-reported unless evidence_status=verified.",
    }
