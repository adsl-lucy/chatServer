#include "tcpConnection.h"
#include <iostream>
#include <string>
#include <pthread.h>
#include <sys/epoll.h>
#include "sync.h"

void* readThread(void* arg);

tcpConnection client;

MutexLock mutex;

int main()
{
	int port;
	string name;
	cout << "sign up a name:";
	cin >> name;
	cout << "choose a port:" ;
	cin >> port;
	while(client.bind(port) == -1){
		cout<< "the port has been used. choose another port:";
		cin >> port;
	}
	if(client.connect(SERVER_PORT,"127.0.0.1") == -1)
		exit(0);
	else
		cout<< "welcome to the chatroom" << endl;

	pthread_t t1;
	if(pthread_create(&t1,nullptr,readThread,nullptr) == -1)
	{
		cerr << "thread error" << endl;
		exit(0);
	}


	string msghead = name + ':';
	cin.ignore();

	for(;;){
		string msg;
		cout << "put in your msg:";
		getline(cin,msg);
		msg = msghead + msg;
		{
			MutexLockGuard lock(mutex);
			client.write(msg);
		}
	}
	return 0;
}

void* readThread(void* arg)
{
	int epfd,ready;
	struct epoll_event ev;
	struct epoll_event evlist[1];
	epfd = epoll_create(1);
	ev.data.fd = client.showFd();
	ev.events = EPOLLIN | EPOLLRDHUP;

	epoll_ctl(epfd, EPOLL_CTL_ADD, client.showFd(), &ev);


	for(;;){
	       	epoll_wait(epfd, evlist,1,-1);
		if(evlist[0].events & EPOLLIN){
			MutexLockGuard lock(mutex);
			cout << "\nnew msg:" << endl;
			string msg = client.read();
			cout << msg << endl;
		}
	}
	return nullptr;
}

