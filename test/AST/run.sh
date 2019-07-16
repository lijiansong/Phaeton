#!/bin/bash
set -e
EXE=../../build/bin/ph-astdump
for tc in `ls *.ph`
do
    echo "===----- Begin test ${tc} -----==="
    ${EXE} ${tc}
    echo "===----- AST dump end! -----==="
done
