cmake_minimum_required(VERSION 2.8.4)
project(HNOCS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    ~/omnetpp-4.6/include
    src
    src/routers/
    src/routers/flat
    src/routers/hier
    src/routers/hier/inPort
    src/routers/hier/opCalc
    src/routers/hier/opCalc/static
    src/routers/hier/sched
    src/routers/hier/sched/wormhole
)

file(GLOB libs "~/omnetpp-4.6/lib/*.sp")

set(SOURCE_FILES
    src/cores/central/CSched.cc
    src/cores/central/CSched.h
    src/cores/central/ParallelCentralSched.cc
    src/cores/central/ParallelCentralSched.h
    src/cores/sinks/AppSink.cc
    src/cores/sinks/AppSink.h
    src/cores/sinks/InfiniteBWMultiVCSink.cc
    src/cores/sinks/InfiniteBWMultiVCSink.h
    src/cores/sinks/InfiniteBWMultiVCSinkperSrc.cc
    src/cores/sinks/InfiniteBWMultiVCSinkperSrc.h
    src/cores/sources/AppSrc.cc
    src/cores/sources/AppSrc.h
    src/cores/sources/CAppSrc.cc
    src/cores/sources/CAppSrc.h
    src/cores/sources/CentSchedPktFifoSrc.cc
    src/cores/sources/CentSchedPktFifoSrc.h
    src/cores/sources/PktFifoSrc.cc
    src/cores/sources/PktFifoSrc.h
    src/cores/CoreL1.cc
    src/cores/CoreL1.h
    src/cores/DRAM.cc
    src/cores/DRAM.h
    src/cores/EchoApp.cc
    src/cores/EchoApp.h
    src/cores/L2.cc
    src/cores/L2.h
    src/routers/flat/predictors/AveragePredictor.h
    src/routers/flat/predictors/NoPredictor.h
    src/routers/flat/predictors/Predictor.h
    src/routers/flat/predictors/PredictorFactory.h
    src/routers/flat/predictors/Random.h
    src/routers/flat/predictors/TCPPredictor.h
    src/routers/flat/CentSchedRouter.cc
    src/routers/flat/CentSchedRouter.h
    src/routers/flat/DoubleBufferFlatPort.cc
    src/routers/flat/DoubleBufferFlatPort.h
    src/routers/flat/FlatPort.cc
    src/routers/flat/FlatPort.h
    src/routers/flat/InterRouterMatcher.h
    src/routers/flat/InterRouterMatchers.cc
    src/routers/flat/InterRouterMatchers.h
    src/routers/flat/SourceFlatPort.cc
    src/routers/flat/SourceFlatPort.h
    src/App_m.cc
    src/App_m.h
    src/Central_m.cc
    src/Central_m.h
    src/CMP_m.cc
    src/CMP_m.h
    src/cResponseHeadArrived.h
    src/NoCs_m.cc
    src/NoCs_m.h
    src/ResponseDB.cc
    src/ResponseDB.h
    src/SessionMeta.cc
    src/SessionMeta.h
    src/Utils.cc
    src/Utils.h
    src/routers/hier/FlitMsgCtrl.h
    )


add_executable(HNOCS ${SOURCE_FILES})


target_link_libraries(HNOCS /home/doron/omnetpp-4.6/lib/liboppcmdenvd.so
                            /home/doron/omnetpp-4.6/lib/liboppcmdenv.so
                            /home/doron/omnetpp-4.6/lib/liboppcommond.so
                            /home/doron/omnetpp-4.6/lib/liboppcommon.so
                            /home/doron/omnetpp-4.6/lib/liboppenvird.so
                            /home/doron/omnetpp-4.6/lib/liboppenvir.so
                            /home/doron/omnetpp-4.6/lib/liboppeventlogd.so
                            /home/doron/omnetpp-4.6/lib/liboppeventlog.so
                            /home/doron/omnetpp-4.6/lib/libopplayoutd.so
                            /home/doron/omnetpp-4.6/lib/libopplayout.so
                            /home/doron/omnetpp-4.6/lib/liboppnedxmld.so
                            /home/doron/omnetpp-4.6/lib/liboppnedxml.so
                            /home/doron/omnetpp-4.6/lib/liboppscaved.so
                            /home/doron/omnetpp-4.6/lib/liboppscave.so
                            /home/doron/omnetpp-4.6/lib/liboppsimd.so
                            /home/doron/omnetpp-4.6/lib/liboppsim.so
                            /home/doron/omnetpp-4.6/lib/libopptkenvd.so
                            /home/doron/omnetpp-4.6/lib/libopptkenv.so
)