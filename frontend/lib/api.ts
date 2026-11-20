// API client for the frontend.
// Calls the FastAPI backend via Next.js API rewrites (/api -> http://localhost:8000/api).

const BASE_URL = process.env.NEXT_PUBLIC_API_URL || "/api";

export async function apiRequest(
  path: string,
  options: RequestInit = {},
) {
  const url = `${BASE_URL}${path}`;
  const response = await fetch(url, {
    headers: {
      "Content-Type": "application/json",
      ...options.headers,
    },
    ...options,
  });

  if (!response.ok) {
    const error = await response.json().catch(() => ({}));
    throw new Error(error.detail || `API error: ${response.status}`);
  }

  return response.json();
}

export const transactions = {
  list: (params?: Record<string, string>) => {
    const search = params ? `?${new URLSearchParams(params)}` : "";
    return apiRequest(`/v1/transactions${search}`);
  },
};

export const orders = {
  list: (params?: Record<string, string>) => {
    const search = params ? `?${new URLSearchParams(params)}` : "";
    return apiRequest(`/v1/orders${search}`);
  },
};

export const positions = {
  list: () => apiRequest(`/v1/positions`),
};

export const transactionPlans = {
  create: (body: unknown) =>
    apiRequest(`/v1/transaction-plans`, {
      method: "POST",
      body: JSON.stringify(body),
    }),
};

export const aiResearch = {
  memo: (body: unknown) =>
    apiRequest(`/v1/ai/research`, {
      method: "POST",
      body: JSON.stringify(body),
    }),
  signal: (body: unknown) =>
    apiRequest(`/v1/ai/signal`, {
      method: "POST",
      body: JSON.stringify(body),
    }),
  summarize: (body: unknown) =>
    apiRequest(`/v1/ai/summarize`, {
      method: "POST",
      body: JSON.stringify(body),
    }),
  chat: (body: unknown) =>
    apiRequest(`/v1/ai/chat`, {
      method: "POST",
      body: JSON.stringify(body),
    }),
};
