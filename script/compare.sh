#!/bin/bash

# Very simple compare function
# Takes in the names of two executable files that at stored in ../bin
# and writes its output to ../testfiles for testing


ELF=~/elf-lab/bin/elf.o
MAIN=~/elf-lab/bin/main.o
TEMP_FILE=~/elf-lab/testfiles/output.txt
LOCATION=~/elf-lab/testfiles/main_symbol.txt


if [[ $1 = "x" ]] ; then
    echo "Executing program "
    $ELF $MAIN > $TEMP_FILE
    grep "30:" -A 40 $TEMP_FILE > $LOCATION
    rm -f $TEMP_FILE
fi
    
