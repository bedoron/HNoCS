#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import sys
from os import path

hist_bins = 100
upper_thresh = 0.00005

data_colors = { 'global':'g', 'l1-l2':'r', 'l2-l3':'b' }

vectors = {}
vectors_names = {}
histograms = {}

vec_names_to_enum = {} # this will help us create overlay histograms

if(len(sys.argv) != 4):
	print "Error", sys.argv
	print sys.argv[0] + " <vec-file-with path> <sca-file-with-path> <output-dir>"
	exit(-1)

if( not (path.isfile(sys.argv[1]) and path.isfile(sys.argv[2]) and path.exists(sys.argv[3])) ):
	print "Error: file not found"
	print sys.argv
	print sys.argv[0] + " <vec-file-with path> <sca-file-with-path> <output-dir>"
	exit(-1)

vecfile = sys.argv[2]
scafile = sys.argv[1]
outdir = sys.argv[3]
print "Processing",outdir
###### Find all needed phases ###############
try:
	with open(vecfile,"r") as fp:
		state = 0
		for line in fp:
			if "session-phases" in line:
				state = 1
				elements = line.split()
				path = elements[3].split('_')
				if(len(path)>1):
					path = path[-1]
				else:
					path = "NA"

				vectors[elements[1]] = { "x": [] , "y": []}
				tmp_name = elements[2][0:-7]
				vectors_names[elements[1]] = (tmp_name, path)
				if tmp_name not in vec_names_to_enum:
					vec_names_to_enum[tmp_name] = {}
				vec_names_to_enum[tmp_name][path] = elements[1]
			else:
				if state == 1:
					state = 2
				if state == 2:
					segments = line.split()
					if segments[0] in vectors:
						x = float(segments[2])
						y = float(segments[3])
						vectors[segments[0]]["x"].append(x)
						vectors[segments[0]]["y"].append(y)

except IOError:
	print "Crap"
	exit(-1)

###### plot phase graphs #######################
print "Plotting graphs"
for enum in vectors.keys():
	entry = vectors[enum]
	name = vectors_names[enum][0] + '-' + vectors_names[enum][1]
	overlay = vectors_names[enum][0]
	overlay_path = vectors_names[enum][1]
	plot_color = data_colors[overlay_path]

######### Build plots
	fig = plt.figure()
#	fig.suptitle(name)
	ax = fig.add_subplot(111)
	ax.set_title(name)
	ax.plot(entry["x"], entry["y"], color=plot_color)
	plt.savefig(outdir + '/' + name + '_session-phases.png')

######### Build histograms based on vector data
	filtered_y = filter(lambda x:x<=upper_thresh, entry["y"])
	if len(filtered_y) < 1:
		print "filtered session deltas of", name, "is now 0, skipping..."
		continue
	fig = plt.figure()
	
	# Make figure twice as large
	fig_size = fig.get_size_inches()
	fig.set_size_inches( (fig_size[0]*2, fig_size[1]*2) ) 
	
	ax = fig.add_subplot(221)
	ax.set_title(name + ' bins=' + str(hist_bins) + ' thresh=' + str(upper_thresh))
	ax.hist(filtered_y, bins = hist_bins, histtype='stepfilled', color=plot_color, alpha=0.5)
	
	ax = fig.add_subplot(222)
	ax.set_title(name + ' bins=' + str(hist_bins/2) + ' thresh=' + str(upper_thresh))
	ax.hist(filtered_y, bins = hist_bins/2, histtype='stepfilled', color=plot_color, alpha=0.5)

	max_delta = max(entry["y"])
	alternative_thresh = max_delta * 2/3 # Different threshold which depends on max value

	filtered_y = filter(lambda x:x<=alternative_thresh, entry["y"])
	if len(filtered_y) < 1:
		plt.savefig(outdir + '/' + name + '_session-deltas.png') # Nothing else to add
		continue	

	ax = fig.add_subplot(223)
	ax.set_title(name + ' bins=' + str(hist_bins) + ' thresh=' + str(alternative_thresh))
	ax.hist(filtered_y, bins = hist_bins, histtype='stepfilled', color=plot_color, alpha=0.5)

	ax = fig.add_subplot(224)
	ax.set_title(name + ' bins=' + str(hist_bins/2) + ' thresh=' + str(alternative_thresh))
	ax.hist(filtered_y, bins = hist_bins/2, histtype='stepfilled', color=plot_color, alpha=0.5)
	
	plt.savefig(outdir + '/' + name + '_session-deltas.png')

########### Draw overlay histograms ####################
for hist_name, hist_vects in vec_names_to_enum.iteritems():
	fig = plt.figure()
	
	fig_size = fig.get_size_inches()
	fig.set_size_inches( (fig_size[0]*2, fig_size[1]*2) ) 
	
	ax = fig.add_subplot(111)
	ax.set_title(hist_name)
	fails = 0 # how many histograms got filtered out
	for vect_name, vect_index in hist_vects.iteritems():
		vis = 0.5
		if vect_name == 'global':
			vis = 0.3
		plot_color = data_colors[vect_name]

		entry = vectors[vect_index]
		filtered_y = filter(lambda x:x<=upper_thresh, entry["y"])
		if len(filtered_y) < 1:
			print "filtered session deltas of", name, "is now 0, skipping..."
			fails = fails + 1
			continue
		ax.hist(filtered_y, bins = hist_bins, histtype='stepfilled', label=vect_name, alpha=vis, color=plot_color)

	if fails == len(hist_vects):	# Graph was wiped out due to filtering
		print "\t ** overlay", hist_name,"wasn't created due to strict filtering"
		continue

	plt.xlabel('Delta')
	plt.ylabel('appeared')
	plt.legend()
	plt.savefig(outdir + '/' + hist_name + '_session-deltas_overlay.png')

############ Draw all events per router ###################
for hist_name, hist_vects in vec_names_to_enum.iteritems():
	fig = plt.figure()
	
	fig_size = fig.get_size_inches()
	fig.set_size_inches( (fig_size[0]*2, fig_size[1]*2) ) 

	fig.suptitle(hist_name + ' messages type distribution')

	fails = 0 # how many histograms got filtered out
	figure_idx = 2

	for vect_name, vect_index in hist_vects.iteritems():
		current_fig_idx = figure_idx
		if vect_name == 'global':
			current_fig_idx = 1
		entry = vectors[vect_index]
		plot_color = data_colors[vect_name]

		filtered_y = filter(lambda x:x<=upper_thresh, entry["y"])
		if len(filtered_y) < 1:
			print "filtered session deltas of", name, "is now 0, skipping..."
			fails = fails + 1
			continue

		ax = fig.add_subplot(2,2,current_fig_idx)
		ax.hist(filtered_y, bins = hist_bins, histtype='stepfilled', label=vect_name, color=plot_color, alpha=0.5)
		plt.xlabel('Delta')
		plt.ylabel('appeared')
		plt.legend()
		figure_idx += 1

	if fails == len(hist_vects):	# Graph was wiped out due to filtering
		print "\t ** overlay", hist_name,"wasn't created due to strict filtering"
		continue

#	plt.xlabel('Delta')
#	plt.ylabel('appeared')
#	plt.legend()
	plt.savefig(outdir + '/' + hist_name + '_session-deltas_all.png')





####################### FILE ENDS HERE ############################
exit(0)

######## Find all needed deltas ###############
with open(scafile,"r") as fp:
	current = ""
	for line in fp:
		if "session-deltas" in line:
			elements = line.split()
			current = elements[1]
			histograms[current] = { "bins": [], "values": []}
		if "field" in line:
			continue
		if "bin" in line:
			elements = line.split()
			appearances = int(elements[2])
			if(appearances!=0):
				delta = "%.3e" % float(elements[1])
				histograms[current]["bins"].append(delta)
				histograms[current]["values"].append(appearances)

####### plot histogram of deltas ################
for entry in histograms.keys():
	current_session_deltas = histograms[entry]
	N = len(current_session_deltas["bins"])
	ind = np.arange(N)
	width = 1

	fig = plt.figure()
	ax = fig.add_subplot(111)
	ax.set_title(entry)
	ax.set_xticks(ind - 0.5)
	rects1 = ax.bar(ind, current_session_deltas["values"], width, color='b', )
	ax.set_xticklabels( tuple(current_session_deltas["bins"]) )
	plt.setp(ax.get_xticklabels(), rotation=45, fontsize=8)
	plt.savefig(outdir + '/' + entry + '_session-deltas.png')





			




#print vectors[vectors.keys()[0]]
#print "This data regards to key %s" % names[vectors_names.keys()[0]]
#print names
#print histograms
