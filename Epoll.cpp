#include"Epoll.hpp"


namespace netthreads
{
Epoll::Epoll(std::map<int, Event>& events_map, int size)
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
	temp.data.fd=fd;
	if (events_map_.find(fd)!=events_map_.end())
	{
		events_map_ [fd].set_events(new_events);
		int ctl_ret=::epoll_ctl(epfd_, kEpollMod, fd, &temp);

		if (ctl_ret==-1) // TODO
		{
			// should rollback and return false
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
			assert(events_map_.find(fd)!=events_map_.end()); // assert!
			Event& event_ref=events_map_ [fd];
			event_ref.set_revents(events_temp_ [i].events);
		}
		for (auto it=events_map_.begin(); it!=events_map_.end(); ++it)
		{
			(*it).second.handle();
		}
	}
	return num;
}

int Epoll::poll()
{
	return poll(-1);
}

} // namespace netthreads
