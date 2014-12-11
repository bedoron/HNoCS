#
# OMNeT++/OMNEST Makefile for hnocs
#
# This file was generated with the command:
#  opp_makemake -f --deep -o hnocs
#

# Name of target to be created (-o option)
TARGET = hnocs$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -Icores \
    -Icores/central \
    -Icores/sinks \
    -Icores/sources \
    -Irouters \
    -Irouters/flat \
    -Irouters/flat/predictors \
    -Irouters/hier \
    -Irouters/hier/Predictors \
    -Irouters/hier/inPort \
    -Irouters/hier/opCalc \
    -Irouters/hier/opCalc/static \
    -Irouters/hier/sched \
    -Irouters/hier/sched/wormhole \
    -Irouters/hier/vcCalc \
    -Irouters/hier/vcCalc/free \
    -Itopologies

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/ResponseDB.o \
    $O/Utils.o \
    $O/SessionMeta.o \
    $O/cores/DRAM.o \
    $O/cores/EchoApp.o \
    $O/cores/CoreL1.o \
    $O/cores/L2.o \
    $O/cores/central/ParallelCentralSched.o \
    $O/cores/central/CSched.o \
    $O/cores/sinks/InfiniteBWMultiVCSinkperSrc.o \
    $O/cores/sinks/AppSink.o \
    $O/cores/sinks/InfiniteBWMultiVCSink.o \
    $O/cores/sources/AppSrc.o \
    $O/cores/sources/CentSchedPktFifoSrc.o \
    $O/cores/sources/CAppSrc.o \
    $O/cores/sources/PktFifoSrc.o \
    $O/routers/flat/FlatPort.o \
    $O/routers/flat/DoubleBufferFlatPort.o \
    $O/routers/flat/InterRouterMatchers.o \
    $O/routers/flat/SourceFlatPort.o \
    $O/routers/flat/CentSchedRouter.o \
    $O/routers/hier/RouterSWHook.o \
    $O/routers/hier/_Predictor.o \
    $O/routers/hier/_PredictorFactory.o \
    $O/routers/hier/PredictionObject.o \
    $O/routers/hier/Predictors/RandomPredictor.o \
    $O/routers/hier/Predictors/DefaultPredictor.o \
    $O/routers/hier/inPort/InPortSync.o \
    $O/routers/hier/inPort/ISLIPInPort.o \
    $O/routers/hier/inPort/InPortAsync.o \
    $O/routers/hier/opCalc/static/XYOPCalc.o \
    $O/routers/hier/sched/wormhole/SchedAsync.o \
    $O/routers/hier/sched/wormhole/ISLIPSched.o \
    $O/routers/hier/sched/wormhole/SchedSync.o \
    $O/routers/hier/vcCalc/free/FLUVCCalc.o \
    $O/Central_m.o \
    $O/NoCs_m.o \
    $O/CMP_m.o \
    $O/App_m.o

# Message files
MSGFILES = \
    Central.msg \
    NoCs.msg \
    CMP.msg \
    App.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f hnocs hnocs.exe libhnocs.so libhnocs.a libhnocs.dll libhnocs.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h
	$(Q)-rm -f cores/*_m.cc cores/*_m.h
	$(Q)-rm -f cores/central/*_m.cc cores/central/*_m.h
	$(Q)-rm -f cores/sinks/*_m.cc cores/sinks/*_m.h
	$(Q)-rm -f cores/sources/*_m.cc cores/sources/*_m.h
	$(Q)-rm -f routers/*_m.cc routers/*_m.h
	$(Q)-rm -f routers/flat/*_m.cc routers/flat/*_m.h
	$(Q)-rm -f routers/flat/predictors/*_m.cc routers/flat/predictors/*_m.h
	$(Q)-rm -f routers/hier/*_m.cc routers/hier/*_m.h
	$(Q)-rm -f routers/hier/Predictors/*_m.cc routers/hier/Predictors/*_m.h
	$(Q)-rm -f routers/hier/inPort/*_m.cc routers/hier/inPort/*_m.h
	$(Q)-rm -f routers/hier/opCalc/*_m.cc routers/hier/opCalc/*_m.h
	$(Q)-rm -f routers/hier/opCalc/static/*_m.cc routers/hier/opCalc/static/*_m.h
	$(Q)-rm -f routers/hier/sched/*_m.cc routers/hier/sched/*_m.h
	$(Q)-rm -f routers/hier/sched/wormhole/*_m.cc routers/hier/sched/wormhole/*_m.h
	$(Q)-rm -f routers/hier/vcCalc/*_m.cc routers/hier/vcCalc/*_m.h
	$(Q)-rm -f routers/hier/vcCalc/free/*_m.cc routers/hier/vcCalc/free/*_m.h
	$(Q)-rm -f topologies/*_m.cc topologies/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc cores/*.cc cores/central/*.cc cores/sinks/*.cc cores/sources/*.cc routers/*.cc routers/flat/*.cc routers/flat/predictors/*.cc routers/hier/*.cc routers/hier/Predictors/*.cc routers/hier/inPort/*.cc routers/hier/opCalc/*.cc routers/hier/opCalc/static/*.cc routers/hier/sched/*.cc routers/hier/sched/wormhole/*.cc routers/hier/vcCalc/*.cc routers/hier/vcCalc/free/*.cc topologies/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/App_m.o: App_m.cc \
	App_m.h \
	NoCs_m.h
$O/CMP_m.o: CMP_m.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h
$O/Central_m.o: Central_m.cc \
	Central_m.h
$O/NoCs_m.o: NoCs_m.cc \
	NoCs_m.h
$O/ResponseDB.o: ResponseDB.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h
$O/SessionMeta.o: SessionMeta.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	SessionMeta.h
$O/Utils.o: Utils.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	Utils.h \
	routers/hier/FlitMsgCtrl.h
$O/cores/CoreL1.o: cores/CoreL1.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	cores/CoreL1.h
$O/cores/DRAM.o: cores/DRAM.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	cores/DRAM.h
$O/cores/EchoApp.o: cores/EchoApp.cc \
	App_m.h \
	NoCs_m.h \
	cores/EchoApp.h
$O/cores/L2.o: cores/L2.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	cores/L2.h
$O/cores/central/CSched.o: cores/central/CSched.cc \
	Central_m.h \
	cores/central/CSched.h
$O/cores/central/ParallelCentralSched.o: cores/central/ParallelCentralSched.cc \
	Central_m.h \
	cores/central/ParallelCentralSched.h
$O/cores/sinks/AppSink.o: cores/sinks/AppSink.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	cores/sinks/AppSink.h
$O/cores/sinks/InfiniteBWMultiVCSink.o: cores/sinks/InfiniteBWMultiVCSink.cc \
	NoCs_m.h \
	cores/sinks/InfiniteBWMultiVCSink.h
$O/cores/sinks/InfiniteBWMultiVCSinkperSrc.o: cores/sinks/InfiniteBWMultiVCSinkperSrc.cc \
	NoCs_m.h \
	cores/sinks/InfiniteBWMultiVCSinkperSrc.h
$O/cores/sources/AppSrc.o: cores/sources/AppSrc.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	Utils.h \
	cores/sources/AppSrc.h \
	routers/hier/FlitMsgCtrl.h
$O/cores/sources/CAppSrc.o: cores/sources/CAppSrc.cc \
	App_m.h \
	Central_m.h \
	NoCs_m.h \
	cores/sources/CAppSrc.h
$O/cores/sources/CentSchedPktFifoSrc.o: cores/sources/CentSchedPktFifoSrc.cc \
	Central_m.h \
	NoCs_m.h \
	cores/sources/CentSchedPktFifoSrc.h
$O/cores/sources/PktFifoSrc.o: cores/sources/PktFifoSrc.cc \
	NoCs_m.h \
	cores/sources/PktFifoSrc.h
$O/routers/flat/CentSchedRouter.o: routers/flat/CentSchedRouter.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	Utils.h \
	routers/flat/CentSchedRouter.h \
	routers/flat/DoubleBufferFlatPort.h \
	routers/flat/FlatPort.h \
	routers/flat/predictors/NoPredictor.h \
	routers/flat/predictors/Predictor.h \
	routers/flat/predictors/PredictorFactory.h \
	routers/flat/predictors/Random.h \
	routers/hier/FlitMsgCtrl.h
$O/routers/flat/DoubleBufferFlatPort.o: routers/flat/DoubleBufferFlatPort.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	Utils.h \
	routers/flat/DoubleBufferFlatPort.h \
	routers/flat/FlatPort.h \
	routers/flat/SourceFlatPort.h \
	routers/hier/FlitMsgCtrl.h
$O/routers/flat/FlatPort.o: routers/flat/FlatPort.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	Utils.h \
	routers/flat/CentSchedRouter.h \
	routers/flat/FlatPort.h \
	routers/flat/InterRouterMatcher.h \
	routers/flat/InterRouterMatchers.h \
	routers/flat/predictors/Predictor.h \
	routers/hier/FlitMsgCtrl.h
$O/routers/flat/InterRouterMatchers.o: routers/flat/InterRouterMatchers.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	Utils.h \
	routers/flat/CentSchedRouter.h \
	routers/flat/FlatPort.h \
	routers/flat/InterRouterMatcher.h \
	routers/flat/InterRouterMatchers.h \
	routers/flat/predictors/Predictor.h \
	routers/hier/FlitMsgCtrl.h
$O/routers/flat/SourceFlatPort.o: routers/flat/SourceFlatPort.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	Utils.h \
	routers/flat/CentSchedRouter.h \
	routers/flat/FlatPort.h \
	routers/flat/SourceFlatPort.h \
	routers/flat/predictors/Predictor.h \
	routers/hier/FlitMsgCtrl.h
$O/routers/hier/PredictionObject.o: routers/hier/PredictionObject.cc
$O/routers/hier/RouterSWHook.o: routers/hier/RouterSWHook.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	cResponseHeadArrived.h \
	routers/hier/RouterSWHook.h
$O/routers/hier/_Predictor.o: routers/hier/_Predictor.cc
$O/routers/hier/_PredictorFactory.o: routers/hier/_PredictorFactory.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	routers/hier/PredictorIfc.h \
	routers/hier/Predictors/DefaultPredictor.h \
	routers/hier/Predictors/RandomPredictor.h \
	routers/hier/_PredictorFactory.h
$O/routers/hier/Predictors/DefaultPredictor.o: routers/hier/Predictors/DefaultPredictor.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	routers/hier/PredictorIfc.h \
	routers/hier/Predictors/DefaultPredictor.h
$O/routers/hier/Predictors/RandomPredictor.o: routers/hier/Predictors/RandomPredictor.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	routers/hier/PredictorIfc.h \
	routers/hier/Predictors/RandomPredictor.h
$O/routers/hier/inPort/ISLIPInPort.o: routers/hier/inPort/ISLIPInPort.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	routers/hier/FlitMsgCtrl.h \
	routers/hier/HierRouter.h \
	routers/hier/PredictorIfc.h \
	routers/hier/inPort/ISLIPInPort.h
$O/routers/hier/inPort/InPortAsync.o: routers/hier/inPort/InPortAsync.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	Utils.h \
	routers/flat/FlatPort.h \
	routers/flat/predictors/Predictor.h \
	routers/hier/FlitMsgCtrl.h \
	routers/hier/inPort/InPortAsync.h
$O/routers/hier/inPort/InPortSync.o: routers/hier/inPort/InPortSync.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	Utils.h \
	routers/flat/FlatPort.h \
	routers/flat/predictors/Predictor.h \
	routers/hier/FlitMsgCtrl.h \
	routers/hier/inPort/InPortSync.h
$O/routers/hier/opCalc/static/XYOPCalc.o: routers/hier/opCalc/static/XYOPCalc.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	ResponseDB.h \
	SessionMeta.h \
	cResponseHeadArrived.h \
	routers/hier/FlitMsgCtrl.h \
	routers/hier/opCalc/static/XYOPCalc.h
$O/routers/hier/sched/wormhole/ISLIPSched.o: routers/hier/sched/wormhole/ISLIPSched.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	routers/hier/HierRouter.h \
	routers/hier/PredictorIfc.h \
	routers/hier/sched/wormhole/ISLIPSched.h
$O/routers/hier/sched/wormhole/SchedAsync.o: routers/hier/sched/wormhole/SchedAsync.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	routers/hier/HierRouter.h \
	routers/hier/PredictorIfc.h \
	routers/hier/sched/wormhole/SchedAsync.h
$O/routers/hier/sched/wormhole/SchedSync.o: routers/hier/sched/wormhole/SchedSync.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	routers/hier/HierRouter.h \
	routers/hier/PredictorIfc.h \
	routers/hier/sched/wormhole/SchedSync.h
$O/routers/hier/vcCalc/free/FLUVCCalc.o: routers/hier/vcCalc/free/FLUVCCalc.cc \
	App_m.h \
	CMP_m.h \
	NoCs_m.h \
	routers/hier/FlitMsgCtrl.h \
	routers/hier/HierRouter.h \
	routers/hier/PredictorIfc.h \
	routers/hier/vcCalc/free/FLUVCCalc.h

