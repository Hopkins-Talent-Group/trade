"""Async AI tasks: research memo generation, signal generation, summarization."""

from celery import current_task
from app.worker import celery_app


@celery_app.task(bind=True, name="ai.generate_research_memo")
def generate_research_memo_task(self, instrument_id: str, research_question: str):
    """Background task: generate an AI research memo."""
    # TODO: integrate LangChain LLM call
    return {
        "task_id": self.request.id,
        "instrument_id": instrument_id,
        "status": "completed",
        "memo": "Placeholder research memo",
    }


@celery_app.task(bind=True, name="ai.generate_signal")
def generate_signal_task(self, instrument_id: str, strategy_family: str):
    """Background task: generate an AI-assisted signal."""
    # TODO: integrate C++ engine bridge + LLM
    return {
        "task_id": self.request.id,
        "instrument_id": instrument_id,
        "strategy_family": strategy_family,
        "status": "completed",
        "signal": "HOLD",
    }


@celery_app.task(bind=True, name="ai.run_backtest")
def run_backtest_task(self, strategy_id: str, start_date: str, end_date: str):
    """Background task: run a strategy backtest."""
    # TODO: integrate backtest engine
    return {
        "task_id": self.request.id,
        "strategy_id": strategy_id,
        "status": "completed",
    }
