#!/usr/bin/env python3
import csv, os, sys, argparse
from pathlib import Path

parser = argparse.ArgumentParser(description="Compare the last two rows of a time-series CSV")
parser.add_argument("--input", required=True, help="Path to the time-series CSV")
args = parser.parse_args()

ts_csv = Path(args.input)
try:
    with open(ts_csv, newline="") as f:
        reader = csv.DictReader(f)
        header = reader.fieldnames or []
        rows = list(reader)
except FileNotFoundError:
    print(f"ERROR: timeseries CSV not found: {ts_csv}")
    sys.exit(2)

if len(rows) < 2:
    print(f"Only {len(rows)} row(s) in {ts_csv}; nothing to compare. Passing.")
    sys.exit(0)

prev, last = rows[-2], rows[-1]

# Compare all columns except metadata
skip = {"date","run","sha"}
cols = [h for h in header if h not in skip]

diffs = []
for c in cols:
    a = prev.get(c, "")
    b = last.get(c, "")
    # Try numeric compare first
    try:
        ai = int(a)
        bi = int(b)
        if ai != bi:
            diffs.append((c, ai, bi))
    except Exception:
        if a != b:
            diffs.append((c, a, b))

# Write a small summary for convenience
summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
if summary_path:
    with open(summary_path, "a", encoding="utf-8") as s:
        s.write("## Metric comparison (last vs previous)\n\n")
        s.write(f"File: `{ts_csv}`  \n")
        if diffs:
            s.write("| Metric | Previous | Last |\n|---|---:|---:|\n")
            for c, a, b in diffs:
                s.write(f"| {c} | {a} | {b} |\n")
        else:
            s.write("No differences detected.\n")

if diffs:
    print("Differences detected:")
    for c, a, b in diffs:
        print(f" - {c}: {a} -> {b}")
    sys.exit(1)   # fail the job
else:
    print("No differences detected. Passing.")
    sys.exit(0)
