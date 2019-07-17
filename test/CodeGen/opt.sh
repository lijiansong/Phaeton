#!/bin/bash
EXE=../../build/bin/ph-opt
for i in `seq 0 4`
do
    echo "===----- Begin CodeGen for t${i}.ph -----==="
    ${EXE} t${i}.ph > t${i}-opt-autogen.py
    echo "===----- End CodeGen! -----==="
done
