#!/bin/bash
EXE=../../build/bin/tir-translate
for tc in `ls *.tir`
do
    echo "===----- Begin lexer for ${tc} -----==="
    ${EXE} ${tc}
    echo "===----- Lexer end! -----==="
done
