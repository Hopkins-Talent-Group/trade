"""Position state endpoint (read-only)."""

from fastapi import APIRouter, Depends

from app.core.security import require_researcher

router = APIRouter()


@router.get("/positions", summary="Retrieve current position state")
async def get_positions(user: dict = Depends(require_researcher)):
    """Return current position state. Read-only research access."""
    # TODO: connect to database / C++ engine position store
    return {
        "schema_version": "1.0.0",
        "data": [],
        "research_only": True,
    }
