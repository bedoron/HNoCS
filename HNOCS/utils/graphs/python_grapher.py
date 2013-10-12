#!/usr/bin/python
# -*- Mode: Python; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
#
# main.py
# Copyright (C) 2013 HNOCS <hnocs@hnocs-sim>
# 
# python-grapher is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# python-grapher is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import sys
from os import path
import Queue
import threading
import time
import AnalysisThread
import AnalyzePorts
import AnalyzeAllPorts
import AnalyzePortsGlobal
import AnalyzeAllPortsGlobal
import AnalyzeIntegral
import AnalyzeCommon
import AnalyzeCommonGlobal
from FromTo3D import FromTo3D

#import VecParser
import JSONParser
#import sys

# THIS MUST BE HERE !!
import pygtk
import gtk
from matplotlib.figure import Figure
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar

from ShowDataSets import ShowDatasets

database = {}
qLock = threading.Lock()
workQ = Queue.Queue() # TODO- this shouldn't be finite
threads = []
#figQ = Queue.Queue()
#figLock = threading.Lock()

if(len(sys.argv) != 3):
	print "Error", sys.argv
	print sys.argv[0] + " <vec-file-with path> <output-dir>"
	exit(-1)

if( not (path.isfile(sys.argv[1])  ) ):
	print "Error: file not found"
	print sys.argv
	print sys.argv[0] + " <vec-file-with path> <output-dir>"
	exit(-1)

input_file = path.expanduser(sys.argv[1])
outdir = path.expanduser(sys.argv[2])

#parser = VecParser.VecParser(0, workQ, qLock, database, input_file)
#parser.run() # Block until done
parser = JSONParser.JSONParser(0, workQ, qLock, database, input_file)
parser.run()

at = AnalysisThread.AnalyzeGraph(1, workQ, qLock, parser, outdir)
ap = AnalyzePorts.AnalyzePorts(2, workQ, qLock, parser, outdir)
apl = AnalyzeAllPorts.AnalyzeAllPorts(2, workQ, qLock, parser, outdir)

apg = AnalyzePortsGlobal.AnalyzePortsGlobal(2, workQ, qLock, parser, outdir)

apgl = AnalyzeAllPortsGlobal.AnalyzeAllPortsGlobal(2, workQ, qLock, parser, outdir)

ail = AnalyzeIntegral.AnalyzeIntegral(2, workQ, qLock, parser, outdir)

acp = AnalyzeCommon.AnalyzeCommon(2, workQ, qLock, parser, outdir)
acpg = AnalyzeCommonGlobal.AnalyzeCommonGlobal(2, workQ, qLock, parser, outdir)

ft3d = FromTo3D(2, workQ, qLock, parser, outdir)


analyzers = [at, ap, apl, apg, apgl, ail, acp, acpg, ft3d]

ds = ShowDatasets(database, parser, analyzers)
ds.show_windows()


exit(0)
