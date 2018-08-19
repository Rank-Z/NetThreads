#ifndef NETTHREADS_MUTEX_HPP
#define NETTHREADS_MUTEX_HPP

#include<pthread.h>
#include"NT_helper.hpp"


namespace netthreads
{

class Mutex : public help::noncopyable
{
public:
	Mutex() // default fast mutex
	{
		pthread_mutexattr_t attr;
		if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ADAPTIVE_NP)!=0) // fast mutex
			throw help::NT_exception(std::string("pthread_mutexattr_settype fail"));
		if (pthread_mutex_init(&mutex_, &attr)!=0)
			throw help::NT_exception(std::string("phtread_mutex_init fail"));
	}

	explicit Mutex(int)
	{
		if(pthread_mutex_init(&mutex_, NULL)!=0) // PTHREAD_MUTEX_DEFAULT
			throw help::NT_exception(std::string("pthread_mutex_init fail"));
	}

	~Mutex()
	{
		if (pthread_mutex_destroy(&mutex_)!=0)
			throw help::NT_exception(std::string("pthread_mutex_destroy fail"));
	}

	pthread_mutex_t& get_real_mutex()
	{
		return mutex_;
	}

	void lock()
	{
		if (pthread_mutex_lock(&mutex_)!=0)
			throw help::NT_exception(std::string("pthread_mutex_lock fail"));
	}

	void unlock()
	{
		if (pthread_mutex_unlock(&mutex_)!=0)
			throw help::NT_exception(std::string("pthread_destroy_init fail"));
	}


private:
	pthread_mutex_t mutex_;
};

class MutexLock
{
public:
	explicit MutexLock(Mutex& mutex)
		:mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexLock()
	{
		mutex_.unlock();
	}

private:
	Mutex& mutex_;
};

} // namespace netthreads

#endif // !NETTHREADS_MUTEX_HPP

