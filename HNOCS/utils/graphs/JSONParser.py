#import Queue
import threading
import json
#import time
def getRouterId(router_json_entry):
	if router_json_entry and type(router_json_entry)==list and len(router_json_entry)>0:
		return router_json_entry[0]["router_id"]
	else:
		print "Invalid JSON DB Entry"
	return None

class JSONParser (threading.Thread):
	session_types =  ["L1_L2", "L2_DRAM", "GLOBAL"]
	l1_l2 = "L1_L2"
	l2_dram = "L2_DRAM"
	
	def __init__(self, threadID, q, qLock, db, json_file):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.q = q
		self.db = db
		self.json = {}
		self.qLock = qLock
		self.input = json_file
		self.entries = []

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

	def toJsonEntry(self, entry):
		name = entry[0]
		router_id = int(name.split("_")[1])
		return router_id

	def parse_file(self, q, qLock, db):
		json_txt = ""
		with open(self.input) as fp:
			for line in fp:
				json_txt += line
				
		tmp_db = json.loads(json_txt)
		self.json = tmp_db
		# Typical line in router should look like:
		# [ router1, router2, ...... ]
		# where each is 
		# [ .... , 
		#	{	"router_id": 0, "session_id": 2511, "session_type": "L2_DRAM", "response_ts": 0.000449854, 
		#	"request_ts": 0.000449374, "request_out_port": 2, "response_out_port": 2 }, ...... ]
		
		
		# Start constructing database with the following heirarchy:
		# db[ROUTER_ID][TRANSACTION_TYPE] = { "x": [....], "y": [.....] }
		# where:	x is the time the transaction returned
		# 			y is the delta between arrival and return
		
		# This part build the legacy DB from the JSON DB
		for router in tmp_db:
			current_key = "router_"+str(router[0]["router_id"]);
			db[current_key] = {"GLOBAL": {"x":[], "y":[]}}
			for entry in router:
				transaction_type = entry["session_type"]
				
				if transaction_type not in db[current_key]:
					db[current_key][transaction_type] = {"x":[], "y":[]}
					qLock.acquire()
					q.put((current_key, transaction_type))
					qLock.release()
					self.entries.append((current_key, transaction_type))
				
				y = entry["response_ts"] - entry["request_ts"]
				x = entry["response_ts"]
				
				db[current_key][transaction_type]["x"].append(x)
				db[current_key][transaction_type]["y"].append(y)
				
				db[current_key]["GLOBAL"]["x"].append(x)
				db[current_key]["GLOBAL"]["y"].append(y)


		