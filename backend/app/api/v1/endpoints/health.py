"""Health check endpoints."""

from fastapi import APIRouter

router = APIRouter()


@router.get("", summary="Health check")
async def health():
    return {
        "status": "ok",
        "research_only": True,
        "autonomous_execution": False,
    }
