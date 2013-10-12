from AnalyzeIfc import AnalyzeIfc
from JSONParser import getRouterId
from mpl_toolkits.mplot3d import  Axes3D
import matplotlib.pyplot as plt

class FromTo3D(AnalyzeIfc):
    
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

            fig1 = plt.figure()
            fig1.suptitle(self.name + " " + entry[0] + "::Common from port " + entry[1])
            ax1 = fig1.add_subplot(111, projection='3d')
        

#             fig2 = plt.figure()
#             fig2.suptitle(self.name + " " + entry[0] + "::Common to port " + entry[1])
#             ax2 = fig2.add_subplot(111, projection='3d')

            for port in xrange(0,5):
                common_in_outs = []
                for diff_port in xrange(0, 5):
                    tmp = [x for x in router if x["response_out_port"] == port and x["request_out_port"] == diff_port and x["session_type"]==entry[1]]
                    common_in_outs.append(len(tmp))

#                 common_out_ins = []
#                 for diff_port in xrange(0, 5):
#                     tmp = [x for x in router if x["request_out_port"] == port and x["response_out_port"] == diff_port and x["session_type"]==entry[1]]
#                     common_out_ins.append(len(tmp))

                
                ax1.bar(range(0,5), common_in_outs, zs=port, zdir='y', color=self.types_legend[entry[1]], alpha=0.8)
#                 ax2.bar(range(0,5), common_out_ins, zs=port, zdir='y', color=self.types_legend[entry[1]], alpha=0.8)
                
            ax1.set_xlabel("Not common port")
            ax1.set_ylabel("Common port")
            ax1.set_zlabel("Total sessions")
            
            ax1.xaxis.set_data_interval(0,4)
            ax1.zaxis.set_data_interval(0,4)
#             ax2.set_xlabel("Not common port")
#             ax2.set_ylabel("Common port")
#             ax2.set_zlabel("Total sessions")
            
            axs.append(ax1)
#             axs.append(ax2)
            
                        
        return axs


        
        