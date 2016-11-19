#!/bin/bash
# set -e
for i in $( ls ); do
	if [ "$i" != 'Content' ] && [ "$i" != 'build_all.sh' ]
		# then echo "./$i/"; 
		then cd "$i" && make clean && cd ../;
	fi;
done