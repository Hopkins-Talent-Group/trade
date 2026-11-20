"""Celery app for async background tasks (AI jobs, data ingestion, backtests)."""

from celery import Celery

from app.core.config import settings

celery_app = Celery(
    "trade_worker",
    broker=settings.CELERY_BROKER_URL,
    backend=settings.CELERY_RESULT_BACKEND,
    include=["app.tasks.ai_tasks", "app.tasks.data_tasks"],
)

celery_app.conf.update(
    task_serializer="json",
    accept_content=["json"],
    result_serializer="json",
    timezone="UTC",
    enable_utc=True,
    task_track_started=True,
    task_time_limit=3600,
)

if __name__ == "__main__":
    celery_app.start()
