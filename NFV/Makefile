
all:
	 g++ -I ./../Sockets/ -I ./../Server/ -std=c++11 -pthread -c receiveFromClient.cpp -o receiveFromClient.o #TODO remove /Server from include path
	 g++ -std=c++11 -c load_distributer.cpp -o load_distributer.o
	 # g++ -I ./../Sockets/ receiveFromClient.o ./../Sockets/tcp_socket.o 
	
clean:
	rm *.o