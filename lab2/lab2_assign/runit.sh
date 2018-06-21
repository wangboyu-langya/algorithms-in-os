#!/bin/bash

# Author: Hubertus Franke  (frankeh@cims.nyu.edu)
OUTDIR=${1:-.}
shift
SCHED=${*:-../src/sched}
echo "sched=<$SCHED> outdir=<$OUTDIR>"

# if you want -v output  run with ...  ./runit.sh youroutputdir   sched -v 

RFILE=./rfile
INS="0 1 2 3 4 5 6"

SCHPAR="F L S R2 R5 P2 P5"
for f in ${INS}; do
	for s in ${SCHPAR}; do 
		echo "${SCHED} ${SCHEDARGS} -s${s} input${f} ${RFILE}"
		${SCHED} -s${s} input${f} ${RFILE} > ${OUTDIR}/output${f}_${s}
	done
done

