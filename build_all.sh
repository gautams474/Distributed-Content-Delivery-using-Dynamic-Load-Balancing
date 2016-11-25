#!/bin/bash
# set -e
export CPLUS_INCLUDE_PATH=$( pwd )/

for i in $( ls ); do
	if [ "$i" != 'Content' ] && [ "$i" != 'build_all.sh' ] && [ "$i" != 'clean_all.sh' ] && [ "$i" != 'send_script.sh' ]
		then echo "$i"; cd "$i" && make; cd ../;
		echo
	fi;
done

g++ -pthread Client/client.o Sockets/tcp_socket.o -o Client/Client.out

cd NFV/ && g++ -pthread load_distributer.o receiveFromClient.o ./../Sockets/tcp_socket.o ./../Sockets/udp_socket.o -o NFV.out; cd ../;
cd Server/ && g++ -pthread ./../Sockets/udp_socket.o ./../Sockets/tcp_socket.o ./../Server/load.o NFVServer.o -o NFVServer.out; cd ../;