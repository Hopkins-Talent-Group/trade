"""Position state endpoint (read-only)."""

from fastapi import APIRouter, Depends
from typing import List

from app.core.security import require_researcher

router = APIRouter()

# In-memory position store for development. Replace with database persistence.
_position_store: dict = {}

# Seed with one illustrative position
_position_store["INSTR-PSE-MER-001"] = {
    "schema_version": "1.0.0",
    "instrument_id": "INSTR-PSE-MER-001",
    "symbol": "MER",
    "shares_held": 1000,
    "average_cost_per_share": "310.00",
    "currency": "PHP",
    "open_buy_orders": 0,
    "open_sell_orders": 0,
    "available_cash": "500000.00",
    "reserved_for_orders": "0.00",
    "exposure_percent": "38.3",
    "maximum_acceptable_loss": "5000.00",
    "evidence_status": "user_reported",
    "last_updated": "2026-11-19T15:30:00+08:00",
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
        "issues": ["User-reported; not independently verified against broker position report"],
        "confidence": 0.5,
    },
}


@router.get("/positions", summary="Retrieve current position state")
async def get_positions(user: dict = Depends(require_researcher)):
    """Return current position state. Read-only research access."""
    results: List[dict] = list(_position_store.values())

    return {
        "schema_version": "1.0.0",
        "data": results,
        "count": len(results),
        "research_only": True,
        "evidence_note": "All positions are user-reported unless evidence_status=verified.",
    }
