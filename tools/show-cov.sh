#!/bin/sh

[ $1 = "_" ] && EXEC="a.out" || EXEC=$1
[ $2 = "_" ] && PROFDATA="default.profdata" || PROFDATA=$2
[ $3 = "_" ] && PROFRAW="*.profraw" || PROFRAW=$3
[ $4 = "_" ] && MODE="show" || MODE=$4

echo "llvm-profdata merge -sparse $PROFRAW -o $PROFDATA && llvm-cov $MODE $EXEC  -instr-profile=$PROFDATA"

llvm-profdata merge -sparse $PROFRAW -o $PROFDATA && \
    llvm-cov $MODE $EXEC  -instr-profile=$PROFDATA