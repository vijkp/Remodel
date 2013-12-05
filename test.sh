#!/bin/sh

echo "//changed" >> tests/network.c
echo "//changed" >> tests/main.c
echo "//changed" >> tests/sched.h
echo "//changed" >> tests/filesyste.c

cd ./tests/
../remodel $1
cd ../

