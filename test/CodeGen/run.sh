#!/bin/bash
EXE=../../build/bin/ph-translate
for i in `seq 0 4`
do
    echo "===----- Begin CodeGen for t${i}.ph -----==="
    ${EXE} t${i}.ph > t${i}-autogen.py
    echo "===----- End CodeGen! -----==="
done
