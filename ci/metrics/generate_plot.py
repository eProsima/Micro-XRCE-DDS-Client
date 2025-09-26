#!/usr/bin/env python3
import csv, sys, argparse
from pathlib import Path
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description="Generate a plot from a time-series CSV")
parser.add_argument("--input", required=True, help="Path to the cumulative time-series CSV")
parser.add_argument("--output", required=True, help="Path to save the plot PNG")
parser.add_argument("--title", default="", help="Title for the plot")
parser.add_argument("--ylabel", default="", help="Y-axis label")
parser.add_argument("--last", type=int, default=30, help="Number of last runs to plot")
args = parser.parse_args()

ts_csv = Path(args.input)
out_file = Path(args.output)

out_file.parent.mkdir(parents=True, exist_ok=True)

if not ts_csv.exists():
    sys.exit(f"{ts_csv} not found")

rows = []
with ts_csv.open(newline="") as fh:
    reader = csv.DictReader(fh)
    header = reader.fieldnames or []
    for r in reader:
        rows.append(r)

if not rows:
    sys.exit("No rows in time-series CSV")

rows = rows[-(args.last):]

skip = {"date", "run", "sha"}
cols = [h for h in header if h not in skip]

x = range(1, len(rows)+1)
dates = [r["date"] for r in rows]

plt.figure()
for key in cols:
    try:
        ys = [int(r.get(key, 0)) for r in rows]
    except Exception:
        continue
    plt.plot(x, ys, label=key, marker="o")

plt.ylim(bottom=0)
plt.grid(True, which="major", axis="y", linestyle=":", alpha=0.5)

plt.title(args.title)
plt.ylabel(args.ylabel)

# Thin x labels for readability
step = max(1, len(dates)//8)
shown = list(range(0, len(dates), step))
plt.xticks([i+1 for i in shown], [dates[i] for i in shown], rotation=45, ha="right")

plt.legend()
plt.tight_layout()
plt.savefig(out_file)
print(f"Wrote {out_file}")
