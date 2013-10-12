#! /bin/bash
RES_SUFF="`date +%F_%s`"
RES_DIR="${HOME}/results_${RES_SUFF}"
echo "Creating ${RES_DIR}"
mkdir $RES_DIR

pushd simulations
GRAPHS="`find -name *.png`"
for gfile in $(find -name *.png); do
	TEST_NAME="`echo $gfile | cut -d / -f2`"
	DUMP_PATH="${RES_DIR}/${TEST_NAME}"
	if [ ! -d "$DUMP_PATH" ]; then
		mkdir $DUMP_PATH	
	fi

	mv $gfile $DUMP_PATH	
done

popd

echo $RES_DIR > results_dir
