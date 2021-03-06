#import Queue
import threading
import time

import numpy as np
import matplotlib as mpl
#mpl.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
#import sys
from collections import deque

MaximumThreads = 2
samples_vector_size = 100
data_colors = { 'global':'g', 'l1-l2':'r', 'l2-l3':'b' }
printingLock = threading.Lock() # Printing lock

class AnalyzeGraph (threading.Thread):
	def __init__(self, threadID, q, qLock, parser, output_dir):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.q = q
		self.parser = parser
		self.db = parser.db
		self.json = parser.json
		self.qLock = qLock
		self.output_dir = output_dir
		#self.types_legend = { "l1-l2": "r", "l2-l3": "b", "global":"g" } #Coloring scheme
		self.types_legend = { "L1_L2": "r", "L2_DRAM": "b", "GLOBAL":"g" } #Coloring scheme
		self.exitFlag = False
		self.name = "Avg/Mean"
		#self.figQ = figQ
		#self.figLock = figLock

	def run(self):
		print "Starting", self.name
		self.process_data(self.name, self.q, self.qLock ,self.db)
		print "Exiting", self.name
		
	def process_data(self, threadName, q, qLock,db):
		while not self.exitFlag:
			qLock.acquire()
			if not q.empty():
				data = q.get()
				qLock.release()
				
				printingLock.acquire()
				print "Analyzing", data[0],data[1]
				printingLock.release()

				
				self.analyze(db, data)
				plt.savefig(self.output_dir + '/' + data[0] + data[1] + '_std-phases.png')
				
				printingLock.acquire()
				print "Analyzing", data[0], data[1], "DONE"
				printingLock.release()
			else:
				qLock.release()
			time.sleep(1)	


	def analyze(self, entry_name): # Hook for all analysis methods we need to call
		entry = self.db[entry_name[0]][entry_name[1]]
		return self.average_analysis(entry, entry_name[0], entry_name[1])

	def average_analysis(self, entry, router_name, transaction_type):
			plot_color = self.types_legend[transaction_type]
			moving_window = deque(entry["y"][:samples_vector_size])
			#stds_vector = []	# Moving standard diviation 
			avgs_vector = []	# Moving averages
			mean_vector = []	# Mean vector
			
			total_elements = len(entry["y"])

			std_upper = []  # +stds_vector
			std_lower = []  # -stds_vector

			for i in xrange(samples_vector_size, total_elements):
				std = np.std(moving_window)
				avg = np.average(moving_window)
				mean = np.mean(moving_window)

				std_upper.append(avg+std)
				#lower = max(avg-std, 0)
				std_lower.append(avg-std)
				avgs_vector.append(avg)
				mean_vector.append(mean)
				# Move window	
				moving_window.popleft()
				moving_window.append(entry["y"][i])

			fig = plt.figure()
			ax = fig.add_subplot(211)
			elements_sampled = " - total: " + str(len(entry["x"])) + \
				str(" - WIND:") + str(samples_vector_size)
				
			ax.set_title("moving avg" + router_name + " " + transaction_type+ " " 
				+ elements_sampled)

			if(len(entry["y"][samples_vector_size:]) != len(avgs_vector)):
				print "lengths differ:", len(entry["y"][samples_vector_size:]), \
					len(avgs_vector)
	
		
			ax.plot(entry["x"][samples_vector_size:], avgs_vector , color=plot_color)
			ax.plot(entry["x"][samples_vector_size:],std_lower, entry["x"][samples_vector_size:],std_upper, color="magenta")
			ax.plot(entry["x"][samples_vector_size:], entry["y"][samples_vector_size:], color="cyan", linestyle="none", alpha = 0.1, marker="o")
		

			ax = fig.add_subplot(212)
			ax.set_title("Moving mean")
			
			ax.plot(entry["x"][samples_vector_size:], mean_vector , color=plot_color)
			ax.plot(entry["x"][samples_vector_size:],std_lower, entry["x"][samples_vector_size:],std_upper, color="magenta")
			ax.plot(entry["x"][samples_vector_size:], entry["y"][samples_vector_size:],color="cyan",  linestyle="none",alpha = 0.1, marker="o")
			
			return ax





