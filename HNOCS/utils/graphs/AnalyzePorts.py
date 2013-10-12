from AnalysisThread import AnalyzeGraph
from sys import stdin
# JSON DB Organization
# Typical line in router should look like:
# [ router1, router2, ...... ]
# where each is 
# [ .... , 
#    {    "router_id": 0, "session_id": 2511, "session_type": "L2_DRAM", "response_ts": 0.000449854, 
#    "request_ts": 0.000449374, "request_out_port": 2, "response_out_port": 2 }, ...... ]


class AnalyzePorts(AnalyzeGraph):
    
    def __init__(self, threadID, q, qLock, parser, output_dir):
        AnalyzeGraph.__init__(self, threadID, q, qLock, parser, output_dir)
        self.name = "Ports " + self.name
    
    def getSides(self):
        print "Please enter source dest pair"
        ln = stdin.readline()
        parts = ln.split(" ")
        self.source = int(parts[0])
        self.dest = int(parts[1])
    
    def transactionType(self, entry_name):
        self.trans_type = entry_name[1]
    
    def analyze(self, entry_name):
        self.getSides()
        
        source = self.source
        dest = self.dest
        
        router_id = self.parser.toJsonEntry(entry_name)
        self.transactionType(entry_name)
        trans_type = self.trans_type 
        router = self.json[router_id]

        entry = { "x":[], "y": []}
        
        core = 0
        hits = 0
        total = 0
        
        for session in router:
            store = False
            
            if(trans_type != None):
                if session["session_type"]!=trans_type:
                    continue

            if(session["request_out_port"]==source and session["response_out_port"]==dest):
                hits += 1
                store = True
            elif (session["request_out_port"]==dest and session["response_out_port"]==source):
                hits += 1
                store = True

            total += 1
            if store:
                x = session["response_ts"]
                y = x - session["request_ts"]
                entry["x"].append(x)
                entry["y"].append(y)
        
        print "{",source,dest,"} -", hits, "matches - ", core, "from core.",total,"valid transactions out of", len(router) 
        
        if hits == 0:
            print "No matches, graph will not be plotted"
            return None
        
        return self.average_analysis(entry, entry_name[0], entry_name[1])
    
        
        