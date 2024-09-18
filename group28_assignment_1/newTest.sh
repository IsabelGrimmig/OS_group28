#!/bin/bash

# Simple test of the command interpreter

in="abbabaq"
out="0,3,5;"
in2="bbabbbaq"
out2="2,5;"
in3="aaaabaq"
out3="0,0,0,0,1;"

[[ $(./cmd_int <<< "$in") == "$out"* ]] && echo "PASSED" || echo "FAILED"
[[ $(./cmd_int <<< "$in2") == "$out2"* ]] && echo "PASSED" || echo "FAILED"
[[ $(./cmd_int <<< "$in3") == "$out3"* ]] && echo "PASSED" || echo "FAILED"