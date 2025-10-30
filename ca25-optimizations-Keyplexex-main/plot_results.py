import pandas as pd
import matplotlib.pyplot as plt
import sys

if len(sys.argv) < 2:
    print("Usage: python3 plot_results.py results/results.csv")
    sys.exit(1)

csv = sys.argv[1]
df = pd.read_csv(csv)
for sched in df['schedule'].unique():
    d = df[df['schedule'] == sched]
    fig, ax = plt.subplots()
    for chunk in d['chunk'].unique():
        dd = d[d['chunk'] == chunk]
        ax.plot(dd['threads'], dd['time_ms'], label=f'chunk={chunk}')
    ax.set_xlabel('Threads')
    ax.set_ylabel('Time (ms)')
    ax.set_title(f'Schedule: {sched}')
    ax.legend()
    ax.grid(True)
    plt.savefig(f'assets/speed_{sched}.png')
    print(f'Saved assets/speed_{sched}.png')