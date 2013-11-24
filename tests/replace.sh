#!/bin/sh

files=`ls -l | awk '{print $9}'| grep "\.c"`

for var in $files
do
	echo $var
	sed   's/.*printf.*/printf(\"%s %d: %s()\\n\", __FILE__, __LINE__, __FUNCTION__);/g' $var > abc
	cat abc > $var
	rm -f abc
done 
exit

