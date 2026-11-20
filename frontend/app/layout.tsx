import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "Trade Analysis — Research Workspace",
  description: "AI-powered trading research with human-in-the-loop approval",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body className="min-h-screen">{children}</body>
    </html>
  );
}
