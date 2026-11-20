"""API v1 router — aggregates all endpoint routers."""

from fastapi import APIRouter

from app.api.v1.endpoints import (
    health,
    transactions,
    orders,
    positions,
    transaction_plans,
    ai_research,
)

api_router = APIRouter()

api_router.include_router(health.router, tags=["health"])
api_router.include_router(transactions.router, tags=["transactions"])
api_router.include_router(orders.router, tags=["orders"])
api_router.include_router(positions.router, tags=["positions"])
api_router.include_router(transaction_plans.router, tags=["transaction-plans"])
api_router.include_router(ai_research.router, tags=["ai-research"])
