#!/bin/bash
EMAIL="sbedoron@t2.technion.ac.il yanivbi@tx.technion.ac.il syevg88@t2.technion.ac.il Shady7001@gmail.com haani.assad@gmail.com"
EMAIL="sbedoron@t2.technion.ac.il"

export hnocs_exec="../../out/gcc-debug/src/hnocs"
export hnocs_paths="../../examples:..:../../src"
export ini_list="iniFiles"
export analysis_tool="`pwd`/utils/getCrap.py"
export to_vhdl_trace="`pwd`/utils/toTrace.py"

EMAILMESSAGE="/tmp/emailmessagesimulation.txt"

export NAMES="$1"

rm -f "simulation_is_done"
run_hnocs () {
	export tname="${1#omnetpp_}"
	export tname="${tname%%.ini}"
	export fname="$1"
	$hnocs_exec -r 0 -u Cmdenv -c $tname -n $hnocs_paths $fname
}

run_and_analyze() {
	touch ../run_status_${1}
	echo "[BEGIN ${1}]"
	echo "[BEGIN]" >> ../run_status_${1}
	echo "[** Running ${1} **]" >> ../run_status_${1}
	run_hnocs $1 #> ../run_output_${1} 2> ../run_output_${1}
	echo "[END  ${1}]"
	echo "[END]" >> ../run_status_${1}
}

touch $EMAILMESSAGE
rm -rf results_dir # Cleanup previous results_dir pointer

#pushd simulations

#find -name results | xargs rm -rf # remove previous execution files
rm -rf run_status_*
rm -rf run_output_*

for INI in $NAMES; do
	export DIR="${INI%\/*}"
	export FILE="${INI##*\/}"
	echo ${DIR}
	echo ${FILE}
	pushd ${DIR}
	
	VECFILE="`pwd`/results/${DIR:2}-0.vec"
	SCAFILE="`pwd`/results/${DIR:2}-0.sca"
	OUTDIR="`pwd`/results"

	(run_and_analyze $FILE $SCAFILE $VECFILE $OUTDIR ${DIR:2}) 
	
	popd
done
#popd
