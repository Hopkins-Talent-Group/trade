# AI-Powered Bot Trading Architecture

## Overview

Research-first trading analysis workspace with AI-assisted signal generation, human-in-the-loop trade approval, and a modern web frontend. Built on top of the existing `vitrixLab/trade` C++20 engine and OpenAPI contract.

**Core constraint (from repository posture):**
- Research-only. No autonomous order placement, modification, or cancellation.
- All trade actions require explicit human approval.
- `/api/v1/transaction-plans` submits plans for human review — it does not execute.

---

## Tech Stack

| Layer | Technology | Purpose |
|-------|-----------|---------|
| **Frontend** | Next.js 14 (App Router), TypeScript, Tailwind CSS | Dashboard, approval queue, AI chat |
| **API** | FastAPI (Python 3.11) | REST + WebSocket, OpenAPI 3.1 |
| **C++ Engine** | C++20 (existing) | Risk math, regime filtering, entry validation, RR calc |
| **AI** | LangChain + OpenAI/Anthropic | Research summarization, signal gen, memo drafting |
| **Database** | PostgreSQL + TimescaleDB | Positions, orders, transactions, research findings |
| **Cache/Queue** | Redis | Session cache, Celery task queue, WebSocket pub/sub |
| **Task Worker** | Celery (Python) | Async AI jobs, data ingestion, backtest runs |
| **Container** | Docker + Docker Compose | Local dev and deployment |

---

## Directory Layout (new)

```
trade/
├── docs/                          # existing research docs
├── schemas/                       # existing JSON schemas
├── api/
│   └── openapi.yaml               # existing OpenAPI contract (+ transaction endpoints)
├── backend/                       # NEW: FastAPI application
│   ├── app/
│   │   ├── main.py
│   │   ├── api/
│   │   │   ├── v1/
│   │   │   │   ├── endpoints/
│   │   │   │   │   ├── transactions.py
│   │   │   │   │   ├── orders.py
│   │   │   │   │   ├── positions.py
│   │   │   │   │   ├── transaction_plans.py
│   │   │   │   │   ├── ai_research.py
│   │   │   │   │   └── health.py
│   │   │   │   └── router.py
│   │   ├── core/
│   │   │   ├── config.py
│   │   │   └── security.py
│   │   ├── services/
│   │   │   ├── ai_service.py
│   │   │   ├── market_data.py
│   │   │   └── cpp_engine_bridge.py
│   │   ├── db/
│   │   │   ├── session.py
│   │   │   └── models.py
│   │   └── schemas/
│   │       └── (Pydantic models)
│   ├── requirements.txt
│   └── Dockerfile
├── frontend/                      # NEW: Next.js application
│   ├── app/
│   │   ├── page.tsx
│   │   ├── layout.tsx
│   │   ├── dashboard/
│   │   ├── approvals/
│   │   ├── research/
│   │   └── ai-chat/
│   ├── components/
│   ├── lib/
│   ├── package.json
│   └── Dockerfile
├── cpp_engine/                    # existing C++20 source (include/, src/)
├── tests/                         # existing GoogleTest suite
├── config/
│   └── data-providers.example.json
├── docker-compose.yml             # NEW: service orchestration
├── .env.example                   # NEW: environment variables
└── ARCHITECTURE.md                # this file
```

---

## API Endpoints (existing + new AI)

### Existing (research-only, read-only)

| Endpoint | Method | Purpose |
|----------|--------|---------|
| `/api/v1/instruments` | GET | List verified instruments |
| `/api/v1/market/bars` | GET | OHLCV bars |
| `/api/v1/macro/observations` | GET | Macro observations |
| `/api/v1/events` | GET | Financial events |
| `/api/v1/portfolio/snapshots` | GET | Portfolio snapshots |
| `/api/v1/research/findings` | GET | Research findings |
| `/api/v1/transactions` | GET | Transaction ledger |
| `/api/v1/orders` | GET | Order status (read-only) |
| `/api/v1/positions` | GET | Position state |
| `/api/v1/transaction-plans` | POST | Submit plan for human review |

### New (AI-powered)

| Endpoint | Method | Purpose |
|----------|--------|---------|
| `/api/v1/ai/research` | POST | AI-generated research memo draft |
| `/api/v1/ai/signal` | POST | AI-assisted signal generation (human review required) |
| `/api/v1/ai/summarize` | POST | Summarize news/events into findings |
| `/api/v1/ai/chat` | POST | Conversational research assistant |
| `/api/v1/ai/backtest` | POST | Trigger async backtest job |
| `ws:///api/v1/stream/market` | WebSocket | Real-time market data stream |

---

## Security Boundaries (enforced)

- **No broker credentials in repo.** Use environment variables / secret manager.
- **No API keys committed.** `.env.example` only.
- **No autonomous execution.** `/api/v1/transaction-plans` is plan submission only; execution is a separate, human-gated step outside this repo's scope.
- **All AI outputs labeled.** FACT / ESTIMATE / HYPOTHESIS / MODEL OUTPUT / UNKNOWN per `docs/AI_QUANT_STRATEGY.md`.
- **Human approval required.** Every trade action passes through the approval queue before any external broker interaction.

---

## Next Steps

1. Create `backend/` FastAPI scaffold
2. Create `frontend/` Next.js scaffold
3. Create `docker-compose.yml` and `.env.example`
4. Wire C++ engine bridge (via ctypes/FFI or subprocess)
5. Implement AI research endpoints

---
