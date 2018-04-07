#ifndef NETTHREADS_EPOLL_HPP
#define NETTHREADS_EPOLL_HPP

#include<assert.h>
#include<vector>
#include<map>
#include<sys/epoll.h>
#include<sys/poll.h>
#include"Event.hpp"


namespace netthreads
{

constexpr int kEpollAdd=EPOLL_CTL_ADD;
constexpr int kEpollMod=EPOLL_CTL_MOD;
constexpr int kEpollDel=EPOLL_CTL_DEL;

class Epoll
{
public:

	Epoll(std::map<int, Event>& events_map,int size);

	bool update_event(Event& event);

	bool del_event(int fd);

	bool del_event(Event& event);

	int poll(int timeout);

	int poll();


private:
	int epfd_;
	std::map<int,Event>& events_map_;
	std::vector<epoll_event> events_temp_;
};


Epoll::Epoll(std::map<int, Event>& events_map,int size)
	:
	epfd_(::epoll_create(size)),
	events_map_(events_map),
	events_temp_(size)
{

}

bool Epoll::update_event(Event& event)
{
	int fd=event.get_fd();
	int new_events=event.get_events();
	epoll_event temp;
	temp.events=new_events;

	if (events_map_.find(fd)!=events_map_.end())
	{
		events_map_ [fd].set_events(new_events);
		int ctl_ret=::epoll_ctl(epfd_, kEpollMod, fd, &temp);

		if (ctl_ret==-1) // TODO
		{

		}
	}
	else
	{
		events_map_ [fd]=event;
		int ctl_ret=::epoll_ctl(epfd_, kEpollAdd, fd, &temp);
		
		if (ctl_ret==-1) // TODO
		{

		}
	}

	return true;
}

bool Epoll::del_event(int fd)
{
	auto it=events_map_.find(fd);
	if (it!=events_map_.end())
	{
		events_map_.erase(it);
		int ctl_ret=::epoll_ctl(epfd_, kEpollDel, fd, NULL);
		
		if (ctl_ret==-1) // TODO
		{

		}

		return true;
	}
	else
	{
		return false;
	}
}

bool Epoll::del_event(Event& event)
{
	return del_event(event.get_fd());
}

int Epoll::poll(int timeout)
{
	int num=::epoll_wait(epfd_, events_temp_.data(), events_temp_.size(), timeout);
	if (num<0) // TODO
	{

	}
	else
	{
		for (int i=0; i!=num; ++i)
		{
			int fd=events_temp_ [i].data.fd;
			assert(events_map_.find(fd)!=events_map_.end());
			Event& event_ref=events_map_ [fd];
			event_ref.set_revents(events_temp_ [i].events);
		}
	}
	return num;
}

int Epoll::poll()
{
	return poll(-1);
}


} // namespace netthreads

#endif // !NETTHREADS_EPOLL_HPP
