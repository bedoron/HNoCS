from AnalyzePortsGlobal import AnalyzePortsGlobal



class AnalyzeAllPortsGlobal(AnalyzePortsGlobal):
    def __init__(self, threadID, q, qLock, parser, output_dir):
        AnalyzePortsGlobal.__init__(self, threadID, q, qLock, parser, output_dir)
        self.name = "ALL " + self.name
                    
                    
    def analyze(self, entry_name):
        self.source = 0
        self.dest = 0
        
        str_ttl = self.name + " " + entry_name[0] + ":: GLOBAL"
        
        ax = []
        
        for i in xrange(0,5):
            self.source = i
            for j in xrange(i+1, 5):
                self.dest = j
                ttl = str_ttl + " (" +str(i) + "," + str(j) + ")"
                tmp_ax = AnalyzePortsGlobal.analyze(self, entry_name)
                if tmp_ax == None:
                    continue
                
                fig = tmp_ax.get_figure()
                fig.suptitle(ttl)
                
                ax.append(tmp_ax)
        
        return ax
    
    
    def getSides(self):
        pass
