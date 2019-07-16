#!/bin/bash
EXE=../../build/bin/ph-translate
for tc in `ls *.ph`
do
    echo "===----- Begin semantic checking for ${tc} -----==="
    ${EXE} ${tc}
    echo "===----- End semantic checking! -----==="
done
