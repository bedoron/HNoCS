#!/usr/bin/python
import json
from sys import argv
import os
from os.path import join, getsize

def printHelp():
	print "./exec <sources dir> [simulation output: responseDB_by_router.txt] [BLA]"
	print "\t Specifying BLA will open <sources dir>\sim_output, otherwise it would iterate all the dirs inside sim output"

def main(argv):
	if(len(argv)==1):
		printHelp()
		os._exit(0)

	if((len(argv)>=2) and (argv[1]=="help")):
		printHelp()
		os._exit(0)

	fname = "responseDB_by_router.txt"
	if(len(argv)>=3):
		fname = argv[2]		

	src_path = os.path.abspath(os.path.expanduser(argv[1]));

	if(len(argv)>=4):
		print "Processing ", src_path+"/"+fname
		processFile(src_path, fname)
	else:
		print "Processing",src_path
		processDirs(src_path, fname)


def processFile(src_path, fname):
	input_file = os.path.join(src_path,fname)
	if not os.path.isfile(input_file):
		print fname,"doesn't exists, quitting"
		os._exit(-1)
	
	output_prefix =  input_file
	matlab_output_file = output_prefix + ".m"
	vhdl_output_file = output_prefix + ".txt"

	print "\tMatlab output:",matlab_output_file
	print "\tVHDL output:",vhdl_output_file

	f = open(input_file, "r")
	txt = f.read()
	f.close()
	db = json.loads(txt)

	fmatlab = open(matlab_output_file, "w");
	fvhdl   = open(vhdl_output_file, "w");


	process_data(db, fmatlab, fvhdl)


	fvhdl.close()
	fmatlab.close()	



def processDirs(src_path, fname):
	for root, dirs, files in os.walk(src_path):
		input_file = os.path.join(root,fname)
		proj = os.path.split(root)[1]

		if not os.path.isfile(input_file):
			print "Project",proj,"has no analysis file",fname,"Skipping"
			continue

		output_prefix =  os.path.join(root,proj)
		matlab_output_file = output_prefix + ".m"
		vhdl_output_file = output_prefix + ".txt"
	
		print "\tMatlab output:",matlab_output_file
		print "\tVHDL output:",vhdl_output_file
	
		f = open(input_file, "r")
		txt = f.read()
		f.close()
		db = json.loads(txt)

		fmatlab = open(matlab_output_file, "w");
		fvhdl   = open(vhdl_output_file, "w");
	

		process_data(db, fmatlab, fvhdl)
	

		fvhdl.close()
		fmatlab.close()


def process_data(db, matlab, vhdl):
	counter = 0
	for router in db:
		for port in range(0,5):
			port_entries = filter(lambda entry: entry["request_out_port"]==port, router)
			L1_L2_entries 	= filter(lambda entry: entry["session_type"]=="L1_L2", port_entries)
			L2_DRAM_entries = filter(lambda entry: entry["session_type"]=="L2_DRAM", port_entries)
			
			L1_L2 	= map(lambda entry: str(entry["response_ts"]-entry["request_ts"]), L1_L2_entries)
			L2_DRAM = map(lambda entry: str(entry["response_ts"]-entry["request_ts"]), L2_DRAM_entries)

			dataLine = "router_"+ str(counter)+ "_port_"+str(port)+" = "
			dataLine += "{"
			dataLine += "{" + ";".join(L1_L2) + "}"
			dataLine += ","
			dataLine += "{" + ";".join(L2_DRAM) + "}"
			dataLine += "};\n"

			matlab.write(dataLine)	

		counter = counter + 1

if __name__=="__main__":
	main(argv)





