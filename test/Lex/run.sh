#!/bin/bash
EXE=../../build/bin/tir-lex
for tc in `ls *.tir`
do
    echo "===----- Begin lexer for ${tc} -----==="
    ${EXE} ${tc}
    echo "===----- Lexer end! -----==="
done
