from AnalyzeIfc import AnalyzeIfc
from JSONParser import getRouterId
import matplotlib.pyplot as plt

class AnalyzeCommon(AnalyzeIfc):
    
    def __init__(self, threadID, q, qLock, parser, output_dir):
        AnalyzeIfc.__init__(self,threadID, q, qLock, parser, output_dir)
        self.name = "Common ports"
        
        
    def analyze(self, entry):
        print "Analyzer", self.name, "looking for", entry[0]
        json = self.json
        axs = []
        entry_router_id = int(entry[0].split("_")[1])
        for router in json: #Iterate routers
            router_id = getRouterId(router)
            if router_id==None or router_id != entry_router_id: # Skip routers/bad lines
                continue
            
            print "building graphs for router", router_id             
            datasets = { "Common inputs": {}, "Common outputs":{}}
            
            # Now router is a list of transactions which passed through  
            # this router, wee need to filter them by: 
            # 1. common source
            # 2. common destination
            # 3. build data sets by transaction types
            # 4. convert to array with NaN delimetrs
                        
            for port in xrange(0,5):
                #Common incoming
                common_in = [x for x in router if x["response_out_port"] == port]
                port_ds = self.generateDataSetsByTransactions(common_in)
                datasets["Common inputs"][port] = port_ds
                #Common outgoings
                common_out = [x for x in router if x["request_out_port"] == port]
                port_ds = self.generateDataSetsByTransactions(common_out)
                datasets["Common outputs"][port] = port_ds
            
            # 5. Create figure for each type of transaction
            # 6. Overlay all ports for the same router with same entry[1]
            
            for criterias in datasets.keys():           
#                for session in self.session_types:
                    # For each session and common src+dest do
                session = entry[1]
                
                fig = plt.figure()
                fig.suptitle(self.name + " " + entry[0] + "::" + criterias + " " + session)
                ax = fig.add_subplot(111)
                
                ax.set_title(criterias + " " + session)
                plot_figure = False
                # Populate graphs
                for port, sessions in datasets[criterias].iteritems():
                    if(len(sessions[session])==0):
                        continue
                    plot_figure = True
                    legend = session + " " + criterias + " port " + str(port) 
                    ax.plot(sessions[session], [(port+1) for x in range(len(sessions[session]))], 
                            color = self.types_legend[session], label=legend)
                    
                #ax.yaxis.set_data_interval(0,6) # Forge axis to always show
                ax.set_yticks((0,1,2,3,4,5,6))
                ax.set_yticklabels(('','port 0','port 1','port 2','port 3','port core',''))
                
                if plot_figure:
                    axs.append(ax)
                else:
                    print entry[0] + " " + criterias + " " + session + " is empty, not plotting" 
            
        return axs


        
        