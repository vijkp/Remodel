#!/bin/sh

cd ./tests/
../remodel $1
cd ../
echo "remodel: ls -l tests/.remodel/"
ls -l tests/.remodel/
echo "remodel: cat tests/.remodel/md5hashes"
cat tests/.remodel/md5hashes

