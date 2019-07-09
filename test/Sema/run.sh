#!/bin/bash
EXE=../../build/bin/tir-translate
for tc in `ls *.tir`
do
    echo "===----- Begin semantic checking for ${tc} -----==="
    ${EXE} ${tc}
    echo "===----- End semantic checking! -----==="
done
