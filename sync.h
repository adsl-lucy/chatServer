#ifndef SYNC_H
#define SYNC_H


#include <pthread.h>

class MutexLock{                                             //the Object which control the locking and unlocking of mutex 

public:
	MutexLock():holder_(0)
	{
		pthread_mutex_init(&mutex_, NULL); 
	}

	~MutexLock()
	{
		//assert(holder_ == 0);
		pthread_mutex_destroy(&mutex_);
	}

	bool isLockByThisThread()
	{
		return pthread_equal(holder_,pthread_self());
	}

	void lock()                                           //can only be used by MutexLockGuard
	{
		pthread_mutex_lock(&mutex_);
		holder_ = pthread_self();
	}

	void unlock()                                         
	{
		holder_ = 0;
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t* getPthreadMutex()                    //can only be used by Condition
	{ 
		return &mutex_; 
	}

private:

	pthread_mutex_t mutex_;
	pthread_t holder_;
};

class MutexLockGuard                                         //the class use life to control the locking and unlocking of mutex
{
public:
	explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexLockGuard()
	{ 
		mutex_.unlock(); 
	}

private:
	MutexLock& mutex_;
};

#define MutexLockGuard(x) static_assert(false, "missing mutex guard var name")

class Condition
{
public:
	explicit Condition(MutexLock& mutex):mutex_(mutex)
	{ 
		pthread_cond_init(&pcond_, NULL); 
	}

	~Condition() 
	{
		pthread_cond_destroy(&pcond_); 
	}

	void wait()
	{
		pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
	}

	void notify()
	{
		pthread_cond_signal(&pcond_);
	}

	void notifyAll()
	{
		pthread_cond_broadcast(&pcond_);
	}

private:
	MutexLock& mutex_;
	pthread_cond_t pcond_;
};


#endif
