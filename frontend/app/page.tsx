import Link from "next/link";

export default function Home() {
  return (
    <div className="max-w-6xl mx-auto px-4 py-8">
      <div className="mb-8">
        <h1 className="text-3xl font-bold text-slate-900">Trade Analysis</h1>
        <p className="mt-2 text-slate-600">
          Research-first trading analysis workspace — AI-assisted, human-approved.
        </p>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        <section className="bg-white p-6 rounded-lg shadow border border-slate-200">
          <h2 className="text-lg font-semibold mb-4">Portfolio</h2>
          <p className="text-slate-600 text-sm">
            Read-only portfolio snapshots from the API.
          </p>
          <Link
            href="/dashboard"
            className="mt-4 inline-block text-sm text-blue-600 hover:underline"
          >
            View portfolio →
          </Link>
        </section>

        <section className="bg-white p-6 rounded-lg shadow border border-slate-200">
          <h2 className="text-lg font-semibold mb-4">Transaction Plans</h2>
          <p className="text-slate-600 text-sm">
            Submit plans for human review. No autonomous execution.
          </p>
          <Link
            href="/approvals"
            className="mt-4 inline-block text-sm text-blue-600 hover:underline"
          >
            Review queue →
          </Link>
        </section>

        <section className="bg-white p-6 rounded-lg shadow border border-slate-200">
          <h2 className="text-lg font-semibold mb-4">AI Research</h2>
          <p className="text-slate-600 text-sm">
            AI-assisted research memos and signals. All outputs require human review.
          </p>
          <Link
            href="/research"
            className="mt-4 inline-block text-sm text-blue-600 hover:underline"
          >
            Open research →
          </Link>
        </section>
      </div>

      <div className="mt-8 p-4 bg-amber-50 border border-amber-200 rounded-lg">
        <h3 className="font-semibold text-amber-900 mb-2">Research-only posture</h3>
        <ul className="text-sm text-amber-800 list-disc pl-5 space-y-1">
          <li>No autonomous order placement, modification, or cancellation.</li>
          <li>All trade actions require explicit human approval.</li>
          <li>AI outputs carry evidence classification labels (FACT / ESTIMATE / HYPOTHESIS / UNKNOWN).</li>
          <li>No broker credentials or secrets stored in this repository.</li>
        </ul>
      </div>
    </div>
  );
}
