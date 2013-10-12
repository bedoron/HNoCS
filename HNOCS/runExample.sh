#!/bin/bash
EMAIL="sbedoron@t2.technion.ac.il yanivbi@tx.technion.ac.il syevg88@t2.technion.ac.il Shady7001@gmail.com haani.assad@gmail.com"
EMAIL="sbedoron@t2.technion.ac.il"

export hnocs_exec="../../out/gcc-debug/src/hnocs"
export hnocs_paths="../../examples:..:../../src"
export ini_list="iniFiles"
export analysis_tool="`pwd`/utils/getCrap.py"
export to_vhdl_trace="`pwd`/utils/toTrace.py"

EMAILMESSAGE="/tmp/emailmessagesimulation.txt"

export NAMES=""

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
	run_hnocs $1 > ../run_output_${1} 2> ../run_output_${1}
	echo "[** Analyzing ${1} **]" >> ../run_status_${1}
	$analysis_tool $2 $3 $4
	$to_vhdl_trace $4 > $4/$5_vhdl_out.txt
	echo "[END  ${1}]"
	echo "[END]" >> ../run_status_${1}
}

touch $EMAILMESSAGE
rm -rf results_dir # Cleanup previous results_dir pointer

pushd simulations

find -name results | xargs rm -rf # remove previous execution files
rm -rf run_status_*
rm -rf run_output_*

export NAMES="`find . -maxdepth 2 -mindepth 2 | grep ini`" # Collect simulations
if [ -f "$ini_list" ]; then
	export NAMES="`cat $ini_list`"
fi

for INI in $NAMES; do
	export DIR="${INI%\/*}"
	export FILE="${INI##*\/}"
	
	if [[ "${INI:0:1}" == "#" ]]; then
		echo "Skipping $FILE"
		continue
	fi
	
	pushd ${DIR}
	
	VECFILE="`pwd`/results/${DIR:2}-0.vec"
	SCAFILE="`pwd`/results/${DIR:2}-0.sca"
	OUTDIR="`pwd`/results"
	echo $DIR >> $EMAILMESSAGE
	(run_and_analyze $FILE $SCAFILE $VECFILE $OUTDIR ${DIR:2}) &
	
	popd
done
popd


# Now wait for all jobs to finish and send me an email when done
echo "to monitor outputs run:"
echo 'find . -name "*output*" | xargs tail -f'
echo "waiting for rest of hnocs processes to join..."
for job in `jobs -p`; do
	wait $job
done
echo "all hnocs processes joined, emailing notice"
################################ NOTIFY WERE DONE ##############
touch "simulation_is_done"

./saveResults.sh
RES_DIR="`cat results_dir`"

pushd $RES_DIR
tar -cjf results.tar.bz2 `find -name "*.txt"`
scp results.tar.bz2 sbedoron@t2.technion.ac.il:~/
rm -f results.tar.bz2
popd

SUBJECT="HNoCs Simulation on `hostname` is done"

MSG="""
HNoCs Simulation on `hostname` is done in:
`pwd`

server `ifconfig eth0 | grep -o 'inet addr:\([0-9]\{0,3\}\.\?\)\{4\}\b'`

files archived in:
${RES_DIR}
[--------------------------- output results begin ---------------------------------]

`find . -name "*output*" | xargs tail`

[-----------------------------output results end ----------------------------------]
"""
echo "$MSG" 	>> $EMAILMESSAGE

scp $EMAILMESSAGE sbedoron@t2.technion.ac.il:$EMAILMESSAGE
ssh sbedoron@t2.technion.ac.il /bin/mail -s \"$SUBJECT\" -a results.tar.bz2 \"$EMAIL\" < $EMAILMESSAGE
rm -f $EMAILMESSAGE

