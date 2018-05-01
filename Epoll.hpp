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


} // namespace netthreads

#endif // !NETTHREADS_EPOLL_HPP
