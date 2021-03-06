#import pygtk
import gtk
import threading
from mpl_toolkits.mplot3d import  Axes3D
from matplotlib.figure import Figure
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar




class ShowDatasets(threading.Thread):
    
    def __init__(self, database, parser, analyzer):
        threading.Thread.__init__(self)
        self.db = database
        self.wg = gtk.WindowGroup()
        #self.graph = gtk.Window()
        self.control = gtk.Window()
        self.parser = parser
        self.database = parser.db
        self.buttons = []
        self.analyzer = analyzer
        self.analyzers = None
        self.analyzer_id = None
        self.current_entry = None 
        self.analyzers_win = None
        
        self.graphs = []

        self.wg.add_window(self.control)
        
        self.control.connect("destroy", lambda x: gtk.main_quit())       
        
        self.control_vbox = gtk.VBox()
        
        self.control.add(self.control_vbox)
        
        self.graph_widgets = []
        
        self.win_offset = 0
        self.win_max_offset = 400
        self.win_offset_step = 20
        ##################Control###################
        self.control.set_title("Datasets")
        
        for i in xrange(0, len(parser.entries)):
            str_name = parser.entries[i][0] + "::" + parser.entries[i][1]
            bt = gtk.Button(str_name)
            bt.entry_index = i
            self.control_vbox.add(bt)
            self.buttons.append(bt)
            bt.connect("clicked", self.control_event)
        
        
        #################Analyzers###################    
        if type(analyzer) == list:
            self.analyzers = analyzer
            self.analyzer = analyzer[0]
            self.analyzer_id = 0
            self.buildAnalyzersWin()
            
        if self.analyzers_win != None:
            bt = gtk.Button("Show Analyzers")
            self.control_vbox.add(bt)
            bt.connect("clicked", self.analyzer_show_event)
            
    def buildAnalyzersWin(self):
        self.analyzers_win = gtk.Window()
        self.analyzers_hbox = gtk.HBox()
        
        rd = gtk.RadioButton(None, self.analyzers[0].name)
        rd.entry_index = 0
        rd.connect("clicked", self.analyzer_event)
        self.analyzers_hbox.add(rd)
        
        for i in xrange(1, len(self.analyzers)):
            analyzer = self.analyzers[i]
            str_name = analyzer.name
            curr = gtk.RadioButton(rd, str_name)
            curr.entry_index = i
            curr.connect("clicked", self.analyzer_event)
            self.analyzers_hbox.add(curr)
            
        self.analyzers_hbox.show_all()
        self.analyzers_win.add(self.analyzers_hbox)
        self.analyzers_win.show_all()
        self.wg.add_window(self.analyzers_win)
        control_pos = self.control.get_position()
        self.analyzers_win.move(control_pos[0]+self.control.get_size()[0], control_pos[1])
    
    def analyzer_event(self, widget, callback_data=None):
        self.analyzer = self.analyzers[widget.entry_index]
        self.analyzer_id = widget.entry_index
        print "Active analyzer is", self.analyzer.name
        #self.update_graph()
        
    def update_graph(self):

        print "="*25
        ax = self.analyzer.analyze(self.current_entry)
        
        if ax == None:
            return
        
        if type(ax) != list:
            ax_list = [ ax ]
        else:
            ax_list = ax
        
        for ax in ax_list:
            graph = gtk.Window()
            vbox = gtk.VBox()
                    
            self.graphs.append(graph)
            
            graph.set_default_size(800,600)
            
            name = self.analyzer.name + " " + self.current_entry[0] + " " + self.current_entry[1]
            
            fig = ax.get_figure()
            #ax.plot()
            
            canvas = FigureCanvas(fig)
            toolbar = NavigationToolbar(canvas, graph)
            
            
            vbox.pack_start(toolbar, False, False)
            vbox.pack_start(canvas)
            graph.add(vbox)
            graph.set_title(name)
            self.wg.add_window(graph)
            
            pos_off = self.win_offset % self.win_max_offset
            graph.move(120 + pos_off,100 + pos_off)
            self.win_offset += self.win_offset_step
            graph.show_all()
        
    def control_event(self, widget, callback_data=None):
        entry = self.parser.entries[widget.entry_index]
        self.current_entry = entry
        self.update_graph()
    
    def analyzer_show_event(self, widget, callback_data=None):
#        if self.wg.is_closed(self.analyzers_win):
        self.buildAnalyzersWin()
        self.wg.add_window(self.analyzers_win)
        self.analyzers_win.show_all()
        
    def run(self):
        pass
    
    def show_windows(self):
        self.control_vbox.show_all()
        #self.graph_vbox.show_all()
        self.control.show_all()
        #self.graph.show_all()
        gtk.main()
        