from AnalyzePorts import AnalyzePorts

class AnalyzePortsGlobal(AnalyzePorts):
    def __init__(self, threadID, q, qLock, parser, output_dir):
        AnalyzePorts.__init__(self, threadID, q, qLock, parser, output_dir)
        self.name = "GLOBAL " + self.name
        
    def transactionType(self, entry_name):
        self.trans_type = None # Ignore
        
        
        
    