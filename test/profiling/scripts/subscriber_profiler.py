import msparser
import os
import signal
import subprocess
import sys
import time
from tabulate import tabulate

import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

from matplotlib.ticker import EngFormatter

names = ["sub_ref_rel", "sub_xml_rel", "sub_ref_best_eff", "sub_xml_best_eff"]
names = ["sub_ref_rel"]

descriptions = ["Subscriber using references and reliable streams", "Subscriber using XML and reliable streams", "Subscriber using references and best-effort streams", "Subscriber using XML and best-effort streams",]
descriptions = ["Subscriber using references and reliable streams"]

path_pub = "../../../build/test/profiling/publisher/pub_xml_rel_loop"
path = "../../../build/test/profiling/subscriber/"

topic_sizes = [round(x) for x in np.linspace(1, 3*1024, num=10)]
entities_sizes = [1, 5]

for name, desc in zip(names, descriptions):
    total_usages_steady = []

    for entities_size in entities_sizes:
        for topic_size in topic_sizes:
            if not "best_eff" in name or ("best_eff" in name and topic_size < 488):
                print("Running {:s} for {:d} subscribers and topic size of {:d} bytes".format(name, entities_size, topic_size))

                pub_sp = subprocess.Popen(("{2} {0} {1}".format(topic_size, 1, path_pub)).split(), shell=False)
                sub_sp = subprocess.Popen(
                    ("valgrind --tool=massif --stacks=yes --detailed-freq=1 --max-snapshots=300 --threshold=0.0 --massif-out-file=./massif-subscriber-size_{0}_{1}.out {2} {0} {1}".format(topic_size, entities_size, path+name)).split(), shell=False)
                
                sub_sp.wait()
                pub_sp.terminate()
                
                std_heap_usage = 0
                data = msparser.parse_file('massif-subscriber-size_{0}_{1}.out'.format(topic_size, entities_size))

                snapshot_window = [round(len(data['snapshots'])*0.75), round(len(data['snapshots'])*0.9)]

                aux = [data['snapshots'][i]['mem_stack'] + data['snapshots'][i]['mem_heap'] + data['snapshots'][i]['mem_heap_extra'] for i in range(snapshot_window[0], snapshot_window[1])]

                aux = [data['snapshots'][i]['mem_stack'] + data['snapshots'][i]['mem_heap'] + data['snapshots'][i]['mem_heap_extra'] if i > 2*len(data['snapshots'])/3 else 0 for i in range(0, len(data['snapshots']))]

                peak_index, _ = signal.find_peaks(aux)
                aux = [x['mem_stack'] + x['mem_heap'] + x['mem_heap_extra'] for x in np.array(data['snapshots'])[peak_index]]
                total_usage = np.max(aux)
                
                total_usages_steady.append(total_usage)
            else:
                total_usages_steady.append(0)

            os.system("rm massif-subscriber-size*")

    total_usages_steady = np.reshape(total_usages_steady, (len(entities_sizes), len(topic_sizes)))

    fig = plt.figure()
    ax = plt.axes()

    for data,n in zip(total_usages_steady, entities_sizes):
        plt.plot(topic_sizes, data, 'o-', markersize=1.5, linewidth=1, label="{:d} subs".format(n))

    plt.legend(frameon=False, fontsize='small')
    plt.ylabel("Memory usage: heap + stack (B)")
    plt.xlabel("Topic size (B)")
    plt.suptitle("eProsima XRCE-DDS Client -- Memory consumption", fontsize=14)
    plt.title(desc, fontsize=8)

    ax.set_ylim([7000,15000])

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
