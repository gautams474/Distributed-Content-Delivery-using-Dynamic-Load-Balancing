#!/bin/bash
set -e
for i in $( ls ); do
	if [ "$i" != 'Content' ] && [ "$i" != 'build_all.sh' ] && [ "$i" != 'clean_all.sh' ]
		then cd "$i" && make; cd ../;
	fi;
done
cd NFV/ && g++ -I ./../Sockets/ -I ./../Server/ ./../Server/load.o load_distributer.o receiveFromClient.o ./../Sockets/tcp_socket.o ./../Sockets/udp_socket.o; cd ../
#TODO remove /Server and load.o from above