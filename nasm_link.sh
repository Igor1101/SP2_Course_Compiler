#!/bin/bash
ASMFILE=$1.asm
OFILE=$1.o
FILEOUT=$1
nasm -felf64 -g $ASMFILE
gcc $OFILE  -o $FILEOUT  -lc -static
