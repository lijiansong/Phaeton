#!/bin/bash
#set -e
EXE=../../build/bin/ph-translate
for tc in `ls *.ph`
do
    echo "===----- Begin test ${tc} -----==="
    ${EXE} ${tc} --ast-dump
    echo "===----- AST dump end! -----==="
done
