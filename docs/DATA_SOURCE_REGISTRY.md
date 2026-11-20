# Global and Regional Data Source Registry

**Purpose:** Maintain a source-of-truth directory for macroeconomic, market, issuer and event evidence.  
**Rule:** A URL in this registry is a discovery starting point, not proof that a particular dataset was retrieved or validated.

## Source record requirements

For each retrieval record: source name, exact URL/API endpoint, publisher, dataset/series identifier, publication timestamp, observation period, retrieval timestamp (UTC and local), units, currency, frequency, revision/vintage, license/terms, transformation, and local artifact or immutable reference.

## Regional — primary sources (Philippines; adapt to your market)

| Domain | Source | Starting URL | Use |
|---|---|---|---|
| Monetary policy / rates / FX | Bangko Sentral ng Pilipinas (BSP) | https://www.bsp.gov.ph/ | Policy rate, monetary statistics, FX, banking and financial stability |
| Statistics | Philippine Statistics Authority (PSA) | https://psa.gov.ph/ | CPI, GDP, labor, trade and national accounts |
| Public debt / yields | Bureau of the Treasury (BTr) | https://www.treasury.gov.ph/ | Government securities, auction results, debt and fiscal data |
| Listed market / disclosures | Philippine Stock Exchange (PSE) | https://www.pse.com.ph/ | Market data, listed issuers, disclosures, trading calendar |
| Fiscal policy | Department of Finance (DOF) | https://www.dof.gov.ph/ | Fiscal policy, revenue, debt and policy announcements |
| Corporate regulation | Securities and Exchange Commission (SEC PH) | https://www.sec.gov.ph/ | Issuer filings, corporate actions and regulatory notices |

## Global — primary / institutional sources

| Domain | Source | Starting URL | Use |
|---|---|---|---|
| Global macro / forecasts | IMF | https://www.imf.org/ | WEO, financial stability, country reports |
| Development / macro | World Bank | https://www.worldbank.org/ | Global indicators and country context |
| US policy | Federal Reserve | https://www.federalreserve.gov/ | FOMC decisions, minutes, balance sheet and rates |
| US macro series | FRED | https://fred.stlouisfed.org/ | Time series; retain series IDs and vintage where relevant |
| Euro area policy | European Central Bank | https://www.ecb.europa.eu/ | Policy rates, statements and euro-area data |
| Global energy | International Energy Agency | https://www.iea.org/ | Energy market reports and outlooks |
| Global commodities | World Bank Commodity Markets | https://www.worldbank.org/en/research/commodity-markets | Commodity price series and outlook |
| US issuer filings | SEC EDGAR | https://www.sec.gov/edgar | Public-company filings and structured company data |

## Market data vendors

Vendor feeds (exchange data, broker feeds, commercial APIs, financial portals) must be separately documented with entitlement, latency, adjustment policy, coverage, usage rights and known limitations. Do not scrape or redistribute data contrary to provider terms. Prefer exchange and issuer primary disclosures for material facts.

## Source quality hierarchy

1. Official regulator, central bank, exchange, statistics agency, treasury or issuer filing.
2. Recognized multilateral institution or established data provider with documented methodology.
3. Reputable financial news organization for event discovery and context.
4. Analyst commentary, social posts and forums as leads only; corroborate before use.

## Cross-check policy

- Record conflicting values rather than silently choosing one.
- Preserve revisions and publication vintages; macro series are often revised.
- Distinguish announcement date from effective date and observation period.
- Check market timezone, holidays, stale quotes and currency/unit conversions.
- Every derived metric must identify source inputs and calculation method.
