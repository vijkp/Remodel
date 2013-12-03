#!/bin/sh

echo "//changed" >> tests/drivers.c
echo "//changed" >> tests/sched.h
echo "//changed" >> tests/main.c
cd ./tests/
../remodel $1
cd ../

