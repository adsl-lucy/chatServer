#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <iostream>
#include <unistd.h>
#include <string>
#include <memory>
#include "buffer.h"
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "sync.h"

using namespace std;

const int SERVER_PORT = 9012;

class tcpConnection
{
public:
	tcpConnection()
	{
		socketFd = socket(AF_INET,SOCK_STREAM,0);
	};

	tcpConnection(int fd)
		:socketFd(fd){}

	~tcpConnection()
	{
		close(socketFd);
	};

	const int showFd() const
	{
		return socketFd;
	}

	int bind(int port);

	int connect(int port,string ipAddress);

	void listen();

	shared_ptr<tcpConnection> accept();

	string read();

	void write(string &msg);


private:
	int socketFd;
	MutexLock mutex;
};

int tcpConnection::bind(int port)
{
	int result = 0;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	result = ::bind(socketFd, (sockaddr*)&address, sizeof(address));
	return result;
}

int tcpConnection::connect(int port, string ipAddress)
{
	int result = 0;
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &serveraddr.sin_addr);
	result = ::connect(socketFd, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if(result == -1){
		cerr << "connect error" << endl;
	}
	else{
		cout << "successfully get the connection!" << endl;
	}
	return result;
}

void tcpConnection::listen(){
	::listen(socketFd,10);
}

shared_ptr<tcpConnection> tcpConnection::accept()
{
	int fd = ::accept(socketFd,nullptr,nullptr);
	shared_ptr<tcpConnection> connectPtr(new tcpConnection(fd));
	return connectPtr;
}

void tcpConnection::write(string& msg)
{
	int length = msg.size();
	string bufmsg;
	if(length < 10)
		bufmsg = '0' + to_string(length);
	else if(length < 100)
		bufmsg = to_string(length);
	else
	{
		cerr << " can't write over 100 words!" << endl;
		return;
	}
	bufmsg += msg;
	buffer buf(bufmsg);
	{
		MutexLockGuard lock(mutex);
		buf.write(socketFd,buf.size());
	}
	return;
}

string tcpConnection::read(){
	buffer buf;

	buf.read(socketFd,2);
	string msghead = buf.outputData();
	int length = stoi(msghead);
	buf.clear();
	{
		MutexLockGuard lock(mutex);
		buf.read(socketFd,length);
	}
	return buf.outputData();
}


#endif
