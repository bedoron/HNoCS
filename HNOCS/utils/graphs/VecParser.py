import Queue
import threading
import time

class VecParser (threading.Thread):
	transaction_types =  ["l1-l2", "l2-dram", "global"]
	
	def __init__(self, threadID, q, qLock, db, vec_file):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.q = q
		self.db = db
		self.qLock = qLock
		self.input = vec_file

	def run(self):
		print "** Starting", self.name
		self.process_data(self.q, self.qLock ,self.db)
		print "** Exiting", self.name

	def process_data(self, q, qLock, db):
		self.parse_file(q, qLock, db)
		self.fill_queue(q, qLock, db)

	def fill_queue(self, q, qLock,db):
		qLock.acquire()
		for router, transaction_type in db.iteritems():
			for transaction in transaction_type.keys():
				q.put((router, transaction))
		qLock.release()

	def parse_file(self, q, qLock, db):
		vecfile = self.input
		local_key_map = {}
		lineNr = 0
		with open(vecfile,"r") as fp:
			state = 0
			lastSeg = -1
			for line in fp:
				lineNr = lineNr + 1
#				if lineNr%100000 == 0:
#					print "Reading line Nr:", lineNr
				if "session-phases" in line:
					state = 1
					elements = line.split()
					path = elements[3].split('_') # extract type of transaction from statistics variable name
					if(len(path)>1):
						path = path[-1] # global/l1_l2/l2_l3
					else:
						path = "NA"

					statistic_id = elements[1] 			# Vector ID
					statistic_router_name = elements[2][0:-7]	# Router associated
					statistic_type = path				# Type of stat (l1_l2/l2_l3/global)

					local_key_map[statistic_id] = (statistic_router_name, statistic_type) # Map internal ID to database entries
	
					if statistic_router_name not in db: 	# Create entry for router
						db[statistic_router_name] = {}

					if statistic_type not in db[statistic_router_name]: # Create entry for statistic type
						db[statistic_router_name][statistic_type] = { "x": [], "y": [] }
				else:
					if state == 1:
						state = 2
					if state == 2:
						segments = line.split()
						if segments[0] in local_key_map:
							#Add new XY
							router_name, path = local_key_map[segments[0]] # Translate internal id to DB
							x = float(segments[2])
							y = float(segments[3])
							db[router_name][path]["x"].append(x) # t_2 
							db[router_name][path]["y"].append(y) # delta_t

