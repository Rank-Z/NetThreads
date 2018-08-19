#ifndef NETTHREADS_REACTOR_HPP
#define NETTHREADS_REACTOR_HPP

#include<map>
#include"Epoll.hpp"
#include"Event.hpp"
#include"Timer.h"
#include"TimerHeap.hpp"
#include"NT_helper.hpp"

namespace netthreads
{

template<typename Multiplex>
class Reactor : public help::noncopyable
{
public:

	Reactor(int size);

	~Reactor();

	void loop();

	void stop();

	void nostop();

	bool add_event(Event&);

	bool change_event(Event&);

	bool del_event(Event&);

	bool del_fd(int fd);

	TimerHandle addtimer_at(TimeData,Callback);

	TimerHandle addtimer_after(TimeData, Callback);

	TimerHandle addtimer_after(time_t, long, Callback);

	TimerHandle addtimer_every(TimeData, Callback); 

	TimerHandle addtimer_every(time_t, long, Callback);

	bool cancel_timer(TimerHandle);

private:
	std::map<int, Event> events_map_;
	Multiplex poller_;
	bool stop_;
	TimerHeap timerheap_;
};

template<typename Multiplex>
Reactor<Multiplex>::Reactor(int size)
	:poller_(events_map_,size),
	stop_(false)
{   }

template<typename Multiplex>
Reactor<Multiplex>::~Reactor()
{
	// TODO 
}


template<typename Multiplex>
inline void Reactor<Multiplex>::loop()
{
	while (!stop_)
	{
		int events_num=poller_.poll();
		for (auto &t:events_map_)
		{
			t.second.handle();
		}
	}
}

template<typename Multiplex>
inline bool Reactor<Multiplex>::add_event(Event& event)
{
	return poller_.update_event(event);
}

template<typename Multiplex>
inline bool Reactor<Multiplex>::change_event(Event& event)
{
	return poller_.update_event(event);
}

template<typename Multiplex>
inline bool Reactor<Multiplex>::del_event(Event& event)
{
	return poller_.del_event(event);
}

template<typename Multiplex>
inline bool Reactor<Multiplex>::del_fd(int fd)
{
	return poller_.del_event(fd);
}


template<typename Multiplex>
inline void Reactor<Multiplex>::stop()
{
	stop_=true;
}

template<typename Multiplex>
inline void Reactor<Multiplex>::nostop()
{
	stop_=false;
}



template<typename Multiplex>
TimerHandle Reactor<Multiplex>::addtimer_at(TimeData timedata, Callback cb)
{
	TimerType type=At;
	auto s_ptr=std::make_shared<Timer>(timedata, cb,type);
	timerheap_.addtimer(s_ptr);
	TimerHandle w_ptr=s_ptr;
	
	return w_ptr;
}

template<typename Multiplex>
TimerHandle Reactor<Multiplex>::addtimer_after(TimeData interval, Callback cb)
{
	TimerType type=After;

	std::shared_ptr<Timer> s_ptr(new Timer(interval,cb,type));
	timerheap_.addtimer(s_ptr);
	TimerHandle w_ptr=s_ptr;

	return w_ptr;
}

template<typename Multiplex>
TimerHandle Reactor<Multiplex>::addtimer_after(time_t seconds,long nanoseconds, Callback cb)
{
	TimerType type=After;
	std::shared_ptr<Timer> s_ptr(new Timer(seconds,nanoseconds, cb, type));
	timerheap_.addtimer(s_ptr);
	TimerHandle w_ptr=s_ptr;

	return w_ptr;
}

template<typename Multiplex>
TimerHandle Reactor<Multiplex>::addtimer_every(TimeData interval, Callback cb)
{
	TimerType type=Every;
	std::shared_ptr<Timer> s_ptr(new Timer(interval, cb, type));

	timerheap_.addtimer(s_ptr);
	TimerHandle w_ptr=s_ptr;

	return w_ptr;
}

template<typename Multiplex>
TimerHandle Reactor<Multiplex>::addtimer_every(time_t seconds, long nanoseconds, Callback cb)
{
	TimerType type=Every;
	std::shared_ptr<Timer> s_ptr(new Timer(seconds, nanoseconds, cb, type));

	timerheap_.addtimer(s_ptr);
	TimerHandle w_ptr=s_ptr;

	return w_ptr;
}

template<typename Multiplex>
bool Reactor<Multiplex>::cancel_timer(TimerHandle w_ptr)
{
	std::shared_ptr<Timer> s_ptr=w_ptr.lock();
	if (s_ptr)
	{
		(*s_ptr).cancel();

		return true;
	}
	else
		return false;
}


} // namespace netthreads


#endif // !NETTHREADS_REACTOR_HPP
