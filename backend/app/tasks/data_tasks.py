"""Async data tasks: market data ingestion, macro observation refresh."""

from celery import current_task
from app.worker import celery_app


@celery_app.task(bind=True, name="data.ingest_market_bars")
def ingest_market_bars_task(self, instrument_id: str, interval: str, start: str, end: str):
    """Background task: ingest OHLCV bars from a data provider."""
    # TODO: connect to market data vendor API
    return {
        "task_id": self.request.id,
        "instrument_id": instrument_id,
        "interval": interval,
        "status": "completed",
    }


@celery_app.task(bind=True, name="data.refresh_macro")
def refresh_macro_task(self, series_id: str):
    """Background task: refresh macro observations from a data source."""
    # TODO: connect to FRED / central bank / statistics authority
    return {
        "task_id": self.request.id,
        "series_id": series_id,
        "status": "completed",
    }
