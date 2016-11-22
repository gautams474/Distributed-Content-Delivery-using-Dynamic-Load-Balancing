#!/bin/bash
# set -e
for i in $( ls ); do
	if [ "$i" != 'Content' ] && [ "$i" != 'build_all.sh' ] && [ "$i" != 'clean_all.sh' ] && [ "$i" != 'send_script.sh' ]
		then echo "$i"; cd "$i" && make clean; cd ../;
		echo
	fi;
done