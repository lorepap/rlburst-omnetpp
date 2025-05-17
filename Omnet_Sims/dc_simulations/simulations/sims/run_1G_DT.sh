#!/bin/bash


do_extract () {
    python3 ./extractor_shell_creator.py $1
    pushd ./results/
    bash extractor.sh
    popd
    sleep 5
}

rm -rf results

# create the directory to save extracted_results
bash dir_creator.sh

# Decision Tree runs
echo "\n\n-------------------------------------------"
echo "Running DCTCP_DT"
opp_runall -j50 ../../src/dc_simulations -m -u Cmdenv -c DCTCP_DT -n ..:../../src:../../../inet/src:../../../inet/examples:../../../inet/tutorials:../../../inet/showcases --image-path=../../../inet/images -l ../../../inet/src/INET omnetpp_1G.ini
do_extract dctcp_dt
mkdir logs/dctcp_dt_1G
cp results/*.out logs/dctcp_dt_1G/


# move the extracted results
echo "Moving the extracted results to results_1G"
rm -rf results_1G
mv extracted_results results_1G

python3 sample_qct.py
