#!/bin/bash

TYPE="tournament_2"
INPUT_SIZE="small"
INS=10000000
CONF_FILE=${GRAPHITE_ROOT}/config/advcomp.cfg
BENCH=parsec-blackscholes


	## Get parameters
	# pht_index_bits=$(echo $configs | cut -d'_' -f1)
	# bhr_bits=$(echo $configs | cut -d'_' -f2)
	# pht_length=$(echo $configs | cut -d'_' -f3)
        
        global_pht_index_bits=5
        global_bhr_bits=7
	pht_length=2

	bht_index_bits=10
	bht_length=8
        
	## Run benchmark.
	## Note: the block size must be the same for L1-data, L1-instruction and L2
	OUTDIR="./${BENCH}-Branch-Tournament-1-predictor.sim"
	$BENCHMARKS_ROOT/run-sniper -p ${BENCH} -i ${INPUT_SIZE} -n 1 -d ${OUTDIR} \
	  -c ${CONF_FILE} -s periodicins-stats:$INS \
	  -g --perf_model/branch_predictor/type=${TYPE} \
          -g --perf_model/branch_predictor/pht_index_bits=${global_pht_index_bits} \
          -g --perf_model/branch_predictor/bhr_bits=${global_bhr_bits} \
          -g --perf_model/branch_predictor/pht_length=${pht_length} \
          -g --perf_model/branch_predictor/bht_index_bits=${bht_index_bits} \
          -g --perf_model/brnach_predictor/bht_length=${bht_length} \
