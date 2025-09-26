#!/usr/bin/env python3
import csv, os, sys, argparse
from pathlib import Path
import datetime

MAX_ROWS = 208 # 4 years of weekly runs

parser = argparse.ArgumentParser(description="Append a run's CSV to a time-series file")
parser.add_argument("--input", required=True, help="Path to the source CSV (from memory_test.sh)")
parser.add_argument("--output", required=True, help="Path to the cumulative time-series CSV")
args = parser.parse_args()

src_path = Path(args.input)
ts_path  = Path(args.output)

run = os.environ.get("GITHUB_RUN_NUMBER", "")
sha = os.environ.get("GITHUB_SHA", "")[:8]

if not src_path.exists():
    sys.exit(f"missing {src_path}")

with src_path.open(newline="") as fh:
    reader = csv.DictReader(fh)
    rows = list(reader)
if not rows:
    sys.exit("empty source csv")
row = rows[0]

# Build a new row with metadata and all original columns
new_line = {
    "date": datetime.datetime.now(datetime.timezone.utc).isoformat(timespec="seconds"),
    "run": run,
    "sha": sha,
}
for key, val in row.items():
    try:
        new_line[key] = int(val)
    except Exception:
        new_line[key] = val

# Ensure header is consistent
if not ts_path.exists():
    header = ["date", "run", "sha"] + list(row.keys())
    ts_path.parent.mkdir(parents=True, exist_ok=True)
    with ts_path.open("w", newline="") as out:
        w = csv.DictWriter(out, fieldnames=header)
        w.writeheader()
        w.writerow(new_line)
else:
    with ts_path.open(newline="") as fh:
        reader = list(csv.reader(fh))
        header = reader[0]
        existing = reader[1:]

    # append new row
    existing.append([new_line.get(h, "") for h in header])

    # keep only last N rows
    trimmed = existing[-MAX_ROWS:]

    # rewrite file
    with ts_path.open("w", newline="") as out:
        w = csv.writer(out)
        w.writerow(header)
        w.writerows(trimmed)

print(f"Appended row to {ts_path} (keeping last {MAX_ROWS} rows)")
