import Queue
import threading
import time
import pickle

class DBManager:
	def __init__(self, db, q, qLock, out):
		self.db = db
		self.q = q
		self.qLock = qLock
		self.out = out

	def serialize(self):
		exitState = False
		while not exitState:
			self.qLock.acquire()
			if not self.q.empty():
				data = self.q.get()
				self.qLock.release()
				router = data[0]
				req_type = data[1]
				entry = self.db[router][req_type]
				fname = out + "/" + router + "_" + req_type + ".pkl"
				with open(fname, "w") as fp:
					formation = { "router": router, "type": req_type, "entry": entry }
					pickle.dump(formation, fp)
			else:
				self.qLock.release()
				exitState = True

	def read_file(self, fname):
		with open(fname, "r") as fp:
			formation = pickle.load(fp)
			router = formation["router"]
			req_type = formation["type"]
			entry = formation["entry"]

			self.db[router][req_type] = entry
			self.qLock.acquire()
			self.q.put((router, req_type))
			self.qLock.release()


