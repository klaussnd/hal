#!/usr/bin/python3
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
  
def plot_ir(data):
    colours = ('blue', 'cyan', 'red', 'orange')
    ranges = data.range.unique()
    ir_photodiodes = data.ir_photodiode.unique()
    i = 0
    for r in ranges:
        for ir_photodiode in ir_photodiodes:
            sub_data = data[(data['range'] == r) & (data['ir_photodiode'] == ir_photodiode)]
            lbl = 'IR photodiode ' + ir_photodiode + ', range ' + r
            plt.scatter(sub_data['gain'], sub_data['ir'], color=colours[i], 
                        label=lbl)
            i = i + 1
    plt.xlabel('gain')
    plt.ylabel('IR raw value')
    plt.legend()

def plot_vis(data):
    colours = ('blue', 'red')
    ranges = data.range.unique()
    i = 0
    for r in ranges:
        sub_data = data[data['range'] == r]
        plt.scatter(sub_data['gain'], sub_data['vis'], color=colours[i], 
                    label='Range ' + r)
        i = i + 1
    plt.xlabel('gain')
    plt.ylabel('VIS raw value')
    plt.legend()


if len(sys.argv) <= 1:
    print('No datafile')
    sys.exit(1)

input_file = sys.argv[1]
data = pd.read_csv(input_file)
# mask over-exposed values
data = data.replace(0xffff, np.nan)
plt.figure()
plot_vis(data)
plt.savefig(input_file + '_vis.svg')
plt.figure()
plot_ir(data)
plt.savefig(input_file + '_ir.svg')
plt.show()
