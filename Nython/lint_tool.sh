#!/bin/bash
FILE="lint.txt"
echo Output File: $FILE
BIG_OPTIONS="-format-style='none' *.hpp *.cpp  -- -std=c++17"
echo Using following options | tee $FILE
echo
clang-tidy-6.0 -dump-config | tee -a $FILE
echo
echo Starting General Clean | tee -a $FILE
echo
clang-tidy-6.0 $BIG_OPTIONS  | tee -a $FILE
echo Done | tee -a $FILE
