"""Transaction ledger endpoint (read-only)."""

from fastapi import APIRouter, Query, Depends
from typing import Optional, List
from datetime import datetime, timezone
import uuid

from app.core.security import require_researcher

router = APIRouter()

# In-memory transaction store for development. Replace with database persistence.
_transaction_store: dict = {}

# Seed with one illustrative transaction (user_reported, not verified)
_transaction_store["TXN-20261119-001"] = {
    "schema_version": "1.0.0",
    "transaction_id": "TXN-20261119-001",
    "instrument_id": "INSTR-PSE-MER-001",
    "symbol": "MER",
    "side": "BUY",
    "quantity": 1000,
    "price": "310.00",
    "gross_amount": "310000.00",
    "fees": "1077.50",
    "net_cash_flow": "-311077.50",
    "currency": "PHP",
    "status": "filled",
    "evidence_status": "user_reported",
    "realized_pnl": None,
    "recorded_at": "2026-11-19T15:30:00+08:00",
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
        "issues": ["User-reported; not independently verified against broker contract notes"],
        "confidence": 0.5,
    },
}


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
    results: List[dict] = []
    for txn in _transaction_store.values():
        if instrument_id and txn["instrument_id"] != instrument_id:
            continue
        if status and txn["status"] != status:
            continue
        if from_date and txn["recorded_at"] < from_date.isoformat():
            continue
        if to_date and txn["recorded_at"] > to_date.isoformat():
            continue
        results.append(txn)

    return {
        "schema_version": "1.0.0",
        "data": results,
        "count": len(results),
        "filters": {
            "from": from_date.isoformat() if from_date else None,
            "to": to_date.isoformat() if to_date else None,
            "instrument_id": instrument_id,
            "status": status,
        },
        "research_only": True,
        "evidence_note": "All transactions are user-reported unless evidence_status=verified.",
    }
