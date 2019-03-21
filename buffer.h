#ifndef BUFFER_H
#define BUFFER_H


#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;

class buffer 
{
	public:
		buffer(){}

		buffer(string str):data(str){}

		~buffer(){}

		int read(int fd,int num);             
		int write(int fd,int num);
		int append(string &str)
		{
			data.append(str);
			return data.size();
		}

		const int size() const
		{
			return data.size();
		};

		void clear()
		{
			data.clear();
		};

		string outputData()
		{
			return data;
		}

		void showData()
		{
			cout << data.c_str() << endl;
		}

	private:
		string data;

};

int buffer::read(int fd ,int num)
{
	char buf[num];
	ssize_t numleft = num;
	ssize_t numread = 0;
	ssize_t offset = 0;
	while(numleft > 0){
		numread = ::read(fd,&buf[offset],numleft);
		offset += numread;
		numleft -= numread;
	}
	string msg(buf,num);
	data.append(msg);
	return data.size();
}

int buffer::write(int fd,int num)
{
	if(num > data.size()){
		cerr << "overwrite" << endl;
		return 0;
	}
	const char* buf = data.c_str();
	ssize_t numleft = num;
	ssize_t numwrite = 0;
	ssize_t offset = 0;
	while(numleft > 0){
		numwrite = ::write(fd,&buf[offset],numleft);
		offset += numwrite;
		numleft -= numwrite;
	}
	return offset;
}

#endif
