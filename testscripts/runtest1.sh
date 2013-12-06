#!/bin/bash

cd "../testcases/testcase-1"
echo "cd ../testcases/testcase-1"
echo "//changed" >> "network.c"
echo "adding a line to network.c"
echo "//changed" >> "main.c"
echo "adding a line to main.c"
echo "//changed" >> "sched.c"
echo "adding a line to sched.c"

echo "running remodel"
../../remodel $1
cd ../../testscripts/
echo "cd ../../testscripts"


