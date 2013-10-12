from AnalyzeCommon import AnalyzeCommon


class AnalyzeCommonGlobal(AnalyzeCommon):
    
    def __init__(self, threadID, q, qLock, parser, output_dir):
        AnalyzeCommon.__init__(self,threadID, q, qLock, parser, output_dir)
        self.name = "Global " + self.name
        
        
    def analyze(self, entry):
        fixed_entry = (entry[0], "GLOBAL")
        return AnalyzeCommon.analyze(self, fixed_entry)