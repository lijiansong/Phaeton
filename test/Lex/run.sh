#!/bin/bash
EXE=../../build/bin/ph-translate
for tc in `ls *.ph`
do
    echo "===----- Begin lexer for ${tc} -----==="
    ${EXE} ${tc} --token-dump
    echo "===----- Lexer end! -----==="
done
