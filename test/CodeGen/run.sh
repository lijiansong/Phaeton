#!/bin/bash
EXE=../../build/bin/tir-translate
for i in `seq 0 4`
do
    echo "===----- Begin CodeGen for p${i}.tir -----==="
    ${EXE} p${i}.tir > p${i}-autogen.py
    echo "===----- End CodeGen! -----==="
done
