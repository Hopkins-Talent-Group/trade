"""Trading Analysis Intelligence API — AI-powered bot trading backend.

Research-only posture:
- No autonomous order placement, modification, or cancellation.
- /api/v1/transaction-plans submits plans for human review only.
- All AI outputs carry evidence classification labels.
"""

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.trustedhost import TrustedHostMiddleware

from app.api.v1.router import api_router
from app.core.config import settings

app = FastAPI(
    title="Trading Analysis Intelligence API",
    version="1.0.0",
    description=(
        "Research-only intelligence API for AI-powered trading analysis. "
        "Read-only market data, portfolio snapshots, research findings, "
        "and human-reviewed transaction plans. No autonomous execution."
    ),
    openapi_url="/api/v1/openapi.json",
    docs_url="/api/v1/docs",
    redoc_url="/api/v1/redoc",
)

# Security: restrict hosts in production
if not settings.DEBUG:
    app.add_middleware(
        TrustedHostMiddleware,
        allowed_hosts=settings.ALLOWED_HOSTS,
    )

# CORS: restrict to frontend origin in production
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.CORS_ORIGINS,
    allow_credentials=True,
    allow_methods=["GET", "POST", "PUT", "DELETE", "OPTIONS"],
    allow_headers=["*"],
)

app.include_router(api_router, prefix="/api/v1")


@app.get("/healthz", tags=["health"])
async def healthz():
    """Liveness probe."""
    return {"status": "ok"}


@app.get("/api/v1/health", tags=["health"])
async def health():
    """Readiness probe with dependency checks."""
    return {
        "status": "ok",
        "version": "1.0.0",
        "research_only": True,
        "autonomous_execution": False,
    }
