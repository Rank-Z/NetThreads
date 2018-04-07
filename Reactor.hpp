#ifndef NETTHREADS_REACTOR_HPP
#define NETTHREADS_REACTOR_HPP


#include<map>
#include"Epoll.hpp"
#include"Event.hpp"
#include"helper.hpp"

namespace netthreads
{

template<typename Multiplex>
class Reactor : public noncopyable
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

private:
	std::map<int, Event> events_map_;
	Multiplex poller_;
	bool stop_;
};

template<typename Multiplex>
Reactor<Multiplex>::Reactor(int size)
	:poller_(events_map_,size),
	stop_(false)
{   }

template<typename Multiplex>
Reactor<Multiplex>::~Reactor()
{

}


template<typename Multiplex>
void Reactor<Multiplex>::loop()
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
bool Reactor<Multiplex>::add_event(Event& event)
{
	return poller_.update_event(event);
}

template<typename Multiplex>
bool Reactor<Multiplex>::change_event(Event& event)
{
	return poller_.update_event(event);
}

template<typename Multiplex>
bool Reactor<Multiplex>::del_event(Event& event)
{
	return poller_.del_event(event);
}

template<typename Multiplex>
bool Reactor<Multiplex>::del_fd(int fd)
{
	return poller_.del_event(fd);
}


template<typename Multiplex>
void Reactor<Multiplex>::stop()
{
	stop_=true;
}

template<typename Multiplex>
void Reactor<Multiplex>::nostop()
{
	stop_=false;
}

} // namespace netthreads


#endif // !NETTHREADS_REACTOR_HPP
