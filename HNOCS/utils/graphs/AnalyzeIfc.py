import threading
import numpy as np

class AnalyzeIfc(threading.Thread):
    
    def __init__(self, threadID, q, qLock, parser, output_dir):
        """
        threadID -  the ID of the analysis thread
        q -         system work Queue, in steady state it should contain all 
                    entries(*).
        qLock -     the Queue lock
        parser -    parser which was used to load the JSON DB
        output_dir- if Analyzer is going to output a file, output it to output_dir
        """
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.q = q
        self.parser = parser
        self.db = parser.db
        self.json = parser.json
        self.qLock = qLock
        self.output_dir = output_dir
        #Coloring scheme
        self.types_legend = { "L1_L2": "r", "L2_DRAM": "b", "GLOBAL":"g", "l1-l2": "r", "l2-l3": "b", "global":"g" }
        self.session_types = parser.session_types 
        self.exitFlag = False
        self.name = "AnalyzeIfc"
    
    def run(self, saveFigure = False):
        """ 
        Threading part, should be revised
        """
        self.process_data(self.name, saveFigure)
    
    def process_data(self, saveFigure = False):
        """
        Threading part, should be revised
        """
        pass
        if saveFigure:
            pass # Save figure into file
    
    def analyze(self, entry):
        """ 
        Analyze router entry[0] with traffic type entry[1]
        this function can return a list of ax graphs (they would all be opened)
        or a single ax graph
        ------------ JSON DB Structure -----------------
        JSON DB Organization
        Typical line in router should look like:
        [ router1, router2, ...... ]
        where each is 
        [ .... , 
        {    "router_id": 0, "session_id": 2511, "session_type": "L2_DRAM", "response_ts": 0.000449854, 
        "request_ts": 0.000449374, "request_out_port": 2, "response_out_port": 2 }, ...... ]
        """
        pass
    
    def dataSetFromRouterLine(self, router_line):
        """ Convert JSON format entry to a list which matplotlib understands for use as X axis"""
        dataset = []
        for entry in router_line:
#            if (len(dataset) > 0) and (dataset[-1]<entry["request_ts"]):
                
            dataset.append(entry["request_ts"])
            dataset.append(entry["response_ts"])
            dataset.append(np.NAN)
                   
        return dataset
            
    def generateDataSetsByTransactions(self, filtered_router_line):
        mapped_by_type = {}
        for transaction_type in self.parser.session_types:
            by_transaction = [x for x in filtered_router_line if x["session_type"]==transaction_type]
            data = self.dataSetFromRouterLine(by_transaction)
            mapped_by_type[transaction_type] = data
        
        mapped_by_type["GLOBAL"] = self.dataSetFromRouterLine(filtered_router_line)
         
        return mapped_by_type
        