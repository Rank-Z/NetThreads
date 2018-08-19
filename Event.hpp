#ifndef NETTHREADS_EVENT_HPP
#define NETTHREADS_EVENT_HPP

#include"NT_name.h"

namespace netthreads
{

constexpr int kReadEvent=EPOLLIN|EPOLLPRI;
constexpr int kWriteEvent=EPOLLOUT;
constexpr int kErrorEvent=EPOLLERR;
constexpr int kNoEvent=0;

enum EventType
{
	FD,
	SIG,
	TIME
};


class Event
{
public:

	Event()
		:Event(0)
	{   }

	explicit Event(int fd)
		:fd_(fd),
		events_(0),
		revents_(0),
		readcb_(nullptr),
		writecb_(nullptr),
		errorcb_(nullptr)
	{   }

	Event(const Event& right)
		:fd_(right.get_fd()),
		events_(right.get_events()),
		revents_(right.get_revents()), // FIXME?: 这里把已发生事件集也copy，目的是让用户可以手动触发callback
		readcb_(right.readcb_),
		writecb_(right.writecb_),
		errorcb_(right.errorcb_)
	{   }

	Event& operator=(const Event& right)
	{
		events_=right.get_events();
		revents_=right.get_revents(); // FIXME?: 理由同copy ctor
		// 这里不copy 回调函数，考虑到很多情况下只是更新目标事件集，减轻用户代码负担
		return *this;
	}

	int get_fd() const
	{
		return fd_;
	}

	void set_events(int events)
	{
		events_=events;
	}

	int get_events() const
	{
		return events_;
	}

	void readable()
	{
		events_=kReadEvent;
	}

	void writeable()
	{
		events_=kWriteEvent;
	}

	void set_revents(int revents)
	{
		revents_=revents;
	}

	int get_revents() const
	{
		return revents_;
	}

	void clear_events()
	{
		events_=kNoEvent;
	}

	void clear_revents()
	{
		revents_=kNoEvent;
	}

	void set_readcallback(Callback cb)
	{
		readcb_=cb;
	}

	void set_writecallback(Callback cb)
	{
		writecb_=cb;
	}

	void set_errorcallback(Callback cb)
	{
		errorcb_=cb;
	}

	void handle()
	{
		if ((revents_==0)||(events_==0))
			return;

		if (events_&revents_&kErrorEvent)
		{
			if (errorcb_)
				errorcb_(fd_);
		}
		if (events_&revents_&kReadEvent)
		{
			if (readcb_)
				readcb_(fd_);
		}
		if (events_&revents_&kWriteEvent)
		{
			if (writecb_)
				writecb_(fd_);
		}
		clear_revents();
	}

private:
	EventType eventtype_;
	const int fd_;
	int events_;  // Types of events that cares about
	int revents_; // Types of events that actually occurred
	Callback readcb_;
	Callback writecb_;
	Callback errorcb_;
};






} // namespace netthreads

#endif // !NETTHREADS_EVENT_HPP
