#!/bin/bash
EXE=../../build/bin/ph-lex
for tc in `ls *.ph`
do
    echo "===----- Begin lexer for ${tc} -----==="
    ${EXE} ${tc}
    echo "===----- Lexer end! -----==="
done
