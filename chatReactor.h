#ifndef CHATREACTOR_H
#define CHATREACTOR_H

#include <iostream>
#include <string>
#include <memory>
#include <unistd.h>
#include "buffer.h"
#include "tcpConnection.h"
#include "sync.h"
#include <map>
#include <sys/epoll.h>

using namespace std;

using tcp_ptr = shared_ptr<tcpConnection>;

class chatReactor{
public:
	chatReactor(){
		epollFd = epoll_create(3);
		if(epollFd == -1)
			cerr << "epoll_creat error" << endl;
	}

	~chatReactor(){
		close(epollFd);
	}

	void add(tcp_ptr connection);
	void del(tcp_ptr connection);
	shared_ptr<string> handleEvents();
	void broadcast(string &msg);


private:
	MutexLock mutex;
	int epollFd;
	map<int,tcp_ptr> connectionPool;
};

void chatReactor::add(tcp_ptr connection)
{
	struct epoll_event ev;
	ev.data.fd = connection->showFd();
	ev.events = EPOLLIN | EPOLLRDHUP;
	{
		MutexLockGuard lock(mutex);                                                            //lockpoint
		if(epoll_ctl(epollFd,EPOLL_CTL_ADD, connection->showFd(),&ev) == -1)
			cout << " epoll_ctl error" << endl;
		connectionPool.insert(make_pair(connection->showFd(),connection));
	}
}

void chatReactor::del(tcp_ptr connection)
{
	//MutexLockGuard lock(mutex);                                                                  can't lock here
	if(epoll_ctl(epollFd,EPOLL_CTL_DEL,connection->showFd(),nullptr) == -1)
	{
		cerr << "epoll_ctl delete error" << endl;
		return;
	}
	connectionPool.erase(connection->showFd());
}

void chatReactor::broadcast(string& msg)
{
	MutexLockGuard lock(mutex);                                                                   //lockpoint
	for(auto connect: connectionPool)
		connect.second->write(msg);
}

shared_ptr<string> chatReactor::handleEvents()
{
	shared_ptr<string> msgbuf(new string);
	int ready;
	const int maxEvents = 5;
	struct epoll_event evlist[maxEvents];
	ready = epoll_wait(epollFd,evlist,maxEvents,-1);
	if(ready == -1){
		if (errno != EINTR)
		{
			cerr <<"epoll_wait error" << endl;
			exit(0);
		}
	}

	MutexLockGuard lock(mutex);                                                                   //lockpoint
	for(int i = 0; i < ready ; i++){
		auto pos = connectionPool.find(evlist[i].data.fd);
		tcp_ptr connectionPtr = pos->second;
		if(evlist[i].events & EPOLLRDHUP){
			this->del(connectionPtr);
			cout << "a connecttion has been closed" << endl;
		}
		else if(evlist[i].events & EPOLLIN){
			cout << "get new msg" << endl;
			msgbuf->append(connectionPtr->read());
			if(i<(ready - 1))
				msgbuf->append("\n");
		}
	}
	return msgbuf;
}

#endif
