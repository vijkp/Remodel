#!/bin/sh

echo "//changed" >> testcases/testcase-1/network.c
echo "//changed" >> testcases/testcase-1/main.c
echo "//changed" >> testcases/testcase-1/sched.h
echo "//changed" >> testcases/testcase-1/filesyste.c

cd ./testcases/testcase-1/
../../remodel $1
cd ../../

