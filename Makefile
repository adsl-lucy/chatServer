OBJ = buffer.h tcpConnection.h chatReactor.h sync.h


chatServer:$(OBJ) chatServer.cpp client 
	g++ -pthread chatServer.cpp $(OBJ) -o chatServer

client:$(OBJ) client.cpp
	g++ -pthread client.cpp $(OBJ) -o client
