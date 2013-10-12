from AnalyzeAllPorts import AnalyzeAllPorts

#import Queue
import threading
import time

import numpy as np
import matplotlib as mpl
from debian.debtags import reverse
mpl.use("Agg")
import matplotlib.pyplot as plt
#import matplotlib.mlab as mlab
#import sys
from collections import deque

#MaximumThreads = 2
#samples_vector_size = 100
#data_colors = { 'global':'g', 'l1-l2':'r', 'l2-l3':'b' }
#printingLock = threading.Lock() # Printing lock

class AnalyzeIntegral(AnalyzeAllPorts):
    def __init__(self, threadID, q, qLock, parser, output_dir):
        AnalyzeAllPorts.__init__(self, threadID, q, qLock, parser, output_dir)
        self.name = "Integral " + self.name

    def average_analysis(self, entry, router_name, transaction_type):
        plot_color = self.types_legend[transaction_type]
        new_data_set = { "x": [], "y": []}
        
        
        prev_t1 = -1
        
        print "Before"
        for i in xrange(0,5):
            t2 = entry["x"][i]
            t1 = t2 - entry["y"][i]
            print t1, ",", t2
                        
        
        for i in reversed(xrange(0, len(entry["x"]))):
            t2 = entry["x"][i]
            t1 = t2 - entry["y"][i]
            
            if t2 < prev_t1:
                print "Overlapping dataset for", i
            
            new_data_set["x"].append(t1)
            new_data_set["x"].append(t2)
            new_data_set["y"].append(i)
            new_data_set["y"].append(i)
            
            new_data_set["x"].append(np.NaN)
            new_data_set["y"].append(np.NaN)
            
            
            prev_t1 = t1

        print "After"
        for i in xrange(0,10):
            t2 = new_data_set["x"][i]
            t1 = new_data_set["y"][i]
            print t1, "-", t2
        
        fig = plt.figure()
        ax = fig.add_subplot(111)
        
        
        
        ax.set_title("Integral " + router_name + " " + transaction_type)
        
        ax.plot(new_data_set["x"], new_data_set["y"] , color=plot_color)
        
        ax.grid(True)


        myyfmt = mpl.ticker.ScalarFormatter(useOffset=True)
        myyfmt._set_offset(1e9)
        ax.get_xaxis().set_major_formatter(myyfmt)
        
        return ax
        


