#!/bin/bash
set -e
EXE=../../build/bin/tir-astdump
for tc in `ls *.tir`
do
    echo "===----- Begin test ${tc} -----==="
    ${EXE} ${tc}
    echo "===----- AST dump end! -----==="
done
