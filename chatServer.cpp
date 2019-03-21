#include "chatReactor.h"
#include "tcpConnection.h"
#include <memory>
#include <iostream>
#include <string>
#include <pthread.h>

void* ConnectionThreadFunc(void* arg);

chatReactor reactor;
tcpConnection server;

int main(){
	server.bind(SERVER_PORT);
	server.listen();

	int s;
	pthread_t t1;
	s = pthread_create(&t1,nullptr,ConnectionThreadFunc,nullptr);
	
	for(;;){
	shared_ptr<string> msgptr = reactor.handleEvents();
	if(msgptr->size()){
		cout << "receive msg: " << msgptr->c_str() << endl;
		reactor.broadcast(*msgptr);
		cout << "broadcast over" << endl;
	}
	}
	return 0;
}

void* ConnectionThreadFunc(void* arg)
{
	cout << "chatServer start listening" << endl;
	for(;;)
	{
		tcp_ptr clientptr = server.accept();
		reactor.add(clientptr);
		cout << "get a new Connection" << endl;
	}
	return nullptr;

}
