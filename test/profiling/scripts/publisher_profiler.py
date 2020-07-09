import msparser
import os
import signal
import subprocess
import sys
import time
from tabulate import tabulate

import numpy as np
import matplotlib.pyplot as plt

from matplotlib.ticker import EngFormatter

names = ["pub_ref_rel", "pub_xml_rel", "pub_ref_best_eff", "pub_xml_best_eff"]

descriptions = ["Publisher using references and reliable streams", "Publisher using XML and reliable streams", "Publisher using references and best-effort streams", "Publisher using XML and best-effort streams",]

path = "../../../build/test/profiling/publisher/"

topic_sizes = [round(x) for x in np.linspace(1, 3*1024, num=10)]
entities_sizes = [1, 5, 10, 20]

for name, desc in zip(names, descriptions):
    total_usages_steady = []

    for entities_size in entities_sizes:
        for topic_size in topic_sizes:
            print("Running {:s} for {:d} publishers and topic size of {:d} bytes".format(name, entities_size, topic_size))

            pub_sp = subprocess.Popen(
                ("valgrind --tool=massif --stacks=yes --detailed-freq=1 --max-snapshots=300 --threshold=0.0 --massif-out-file=./massif-publisher-size.out {2} {0} {1}".format(topic_size, entities_size, path+name)).split(), shell=False)
            pub_sp.wait()

            std_heap_usage = 0
            data = msparser.parse_file('massif-publisher-size.out'.format(topic_size))

            snapshot_window = [round(len(data['snapshots'])*0.75), round(len(data['snapshots'])*0.8)]

            aux = [data['snapshots'][i]['mem_stack'] + data['snapshots'][i]['mem_heap'] + data['snapshots'][i]['mem_heap_extra'] for i in range(snapshot_window[0], snapshot_window[1])]
            total_usage = np.mean(aux)
            
            total_usages_steady.append(total_usage)

            os.system("rm massif-publisher-size*")

    total_usages_steady = np.reshape(total_usages_steady, (len(entities_sizes), len(topic_sizes)))

    fig = plt.figure()
    ax = plt.axes()

    for data,n in zip(total_usages_steady, entities_sizes):
        plt.plot(topic_sizes, data, 'o-', markersize=1.5, linewidth=1, label="{:d} pubs".format(n))

    plt.legend(frameon=False, fontsize='small')
    plt.ylabel("Memory usage: heap + stack (B)")
    plt.xlabel("Topic size (B)")
    plt.suptitle("eProsima XRCE-DDS Client -- Memory consumption", fontsize=14)
    plt.title(desc, fontsize=8)

    ax.set_ylim([5000,9000])

    formatter1 = EngFormatter(places=1, sep="\N{THIN SPACE}")  # U+2009
    ax.xaxis.set_major_formatter(formatter1)
    ax.yaxis.set_major_formatter(formatter1)

    plt.grid(b=True, which='major', linestyle='--',
             color='grey', linewidth=0.3)
    plt.grid(b=True, which='minor', linestyle='--',
             color='grey', linewidth=0.3)

    plt.savefig(name + ".svg", dpi=400, bbox_inches='tight')
    plt.savefig(name + ".png", dpi=400, bbox_inches='tight')

    # print(tabulate(zip(topic_sizes, [x/1000 for x in stack_usages_steady], [x/1000 for x in heap_usages_steady], [
    #     x/1000 for x in total_usages_steady]), headers=["Topic size (B)", "Stack Usage (KiB)", "Heap Usage (KiB)", "Total Usage (KiB)"]))
