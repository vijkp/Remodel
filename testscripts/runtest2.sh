#!/bin/bash


cd "../testcases/cyclic_dependency"
echo "cd ../testcases/cyclic_dependency"


echo "running remodel"
../../remodel $1
cd ../../testscripts/
echo "cd ../../testscripts"


