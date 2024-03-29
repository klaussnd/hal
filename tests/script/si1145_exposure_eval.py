#!/usr/bin/python3
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
  
def plot_ir(data, title=None):
    colours = ('blue', 'cyan', 'red', 'orange')
    ranges = data.range.unique()
    ranges.sort()
    ir_photodiodes = data.ir_photodiode.unique()
    ir_photodiodes.sort()
    i = 0
    for r in ranges:
        for ir_photodiode in ir_photodiodes:
            sub_data = data[(data['range'] == r) & (data['ir_photodiode'] == ir_photodiode)]
            fit_data = sub_data[np.isfinite(sub_data['ir'])]
            has_any_data = fit_data.shape[0] > 0
            if not has_any_data:
                continue
            is_fittable = len(fit_data.gain.unique()) > 1
            lbl = 'IR ' + ir_photodiode + ', range ' + r
            if is_fittable:
                coeff = np.polynomial.polynomial.polyfit(fit_data['gain'], fit_data['ir'], 1)
                lbl +=  ': {:.1f}*gain + {:.1f}'.format(coeff[1], coeff[0])
            plt.scatter(sub_data['gain'], 
                        sub_data['ir'],
                        color=colours[i], 
                        label=lbl)
            if is_fittable:
                plt.plot(fit_data['gain'],
                         np.polynomial.polynomial.polyval(fit_data['gain'], coeff),
                         color=colours[i],
                         label='')
                print(lbl)
            i = i + 1
    plt.xlabel('gain')
    plt.ylabel('IR raw value')
    plt.legend()
    if title is not None:
        plt.title(title)

def compute_mean_stddev_for_each_gain(subdata):
    x = []
    y = []
    yerr = []
    gains = sub_data.gain.unique()
    for g in gains:
       x.append(g)
       values = sub_data[sub_data['gain'] == g]['vis']
       y.append(np.mean(values))
       yerr.append(np.std(values))
    return x, y, yerr

def plot_vis(data, title=None):
    colours = ('blue', 'red')
    ranges = data.range.unique()
    ranges.sort()
    i = 0
    for r in ranges:
        sub_data = data[data['range'] == r]
        fit_data = sub_data[np.isfinite(sub_data['vis'])]
        has_any_data = fit_data.shape[0] > 0
        if not has_any_data:
            continue
        is_fittable = len(fit_data.gain.unique()) > 1
        lbl = 'Range ' + r
        if is_fittable:
            coeff = np.polynomial.polynomial.polyfit(fit_data['gain'],fit_data['vis'], 1)
            lbl += ': {:.1f}*gain + {:.1f}'.format(coeff[1], coeff[0])
            if r == 'high':
                lbl += '; 14.5*slope = {:.1f}'.format(coeff[1] * 14.5)
        x, y, yerr = compute_mean_stddev_for_each_gain(sub_data)
        plt.errorbar(x, y, yerr,
                    color=colours[i], 
                    label=lbl,
                    fmt='o')
        if is_fittable:
            plt.plot(fit_data['gain'],
                     np.polynomial.polynomial.polyval(fit_data['gain'], coeff),
                     color=colours[i],
                     label='')
        print(lbl)
        i = i + 1
    plt.xlabel('gain')
    plt.ylabel('VIS raw value')
    plt.legend()
    if title is not None:
        plt.title(title)

def remove_invalid_data(data):
    # mask over-exposed values
    data = data.replace(0xffff, np.nan)
    # remove data that have settings that sometimes give wrong results
    data.loc[(data['gain'] == 128) & (data['range'] == 'high'), 'vis'] = np.nan
    return data

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print('No datafile')
        sys.exit(1)

    input_file = sys.argv[1]
    title = None if len(sys.argv) <= 2 else sys.argv[2]
    
    data = pd.read_csv(input_file)
    data = remove_invalid_data(data)

    plt.figure()
    plot_vis(data, title)
    plt.savefig(input_file + '_vis.svg')
    plt.figure()
    plot_ir(data, title)
    plt.savefig(input_file + '_ir.svg')
    plt.show()
