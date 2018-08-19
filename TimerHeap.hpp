#ifndef NETTHREADS_TIMERHEAP_HPP
#define NETTHREADS_TIMERHEAP_HPP

#include<time.h>
#include<sys/timerfd.h>
#include<string.h>
#include<vector>
#include"Timer.h"
#include"Mutex.hpp"
#include"NT_name.h"
#include"NT_helper.hpp"

namespace netthreads
{

class TimerCompare // min_heap ,so it should be '>'
{
public:
	bool operator()(std::shared_ptr<Timer> left,std::shared_ptr<Timer> right)
	{
		TimeData left_time=(*left).get_time();
		TimeData right_time=(*right).get_time();

		if (left_time.tv_sec>right_time.tv_sec)
			return true;
		else if (left_time.tv_sec<right_time.tv_sec)
			return false;
		else
			return (left_time.tv_nsec>right_time.tv_nsec) ? true : false;
	}
	// if left later than right return ture
};


class TimerHeap
{
public:
	TimerHeap()
		:timerfd_(::timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK))
	{
		if (timerfd_==-1)
		{
			// TODO: log here
		}
	}

	~TimerHeap()
	{
		::close(timerfd_);
	}

	void addtimer(std::shared_ptr<Timer> s_ptr)
	{
		Timer& timer=(*s_ptr);
		if (heap_.empty())
		{
			if (settimer(timer))
				heap_.push_heap(s_ptr);
			else if (timer.is_repeat())
				// FIXME
				// 这里假设需要重复的Timer的时间间隔够大，系统没有极端负载的情况下不会有 "已超时Timer"
			{
				timer.repeat();
				bool flag=settimer(timer);
				assert(flag); // 仅测试版本，同上，测试时检测是否有特殊情况发生
				heap_.push_heap(s_ptr);
			}
		}
		else // heap_ no empty
		{
			std::shared_ptr<Timer> old_top_ptr=heap_.get_top();
			TimerCompare comp;
			if (comp(old_top_ptr, s_ptr)) // new timer earlier than min timer of heap 
			{
				itimerspec zero;
				memset(&zero, 0, sizeof zero);
				::timerfd_settime(timerfd_, 0, &zero, NULL); // cancel timerfd
				if (settimer(timer))
					heap_.push_heap(s_ptr);
				else // already time out,but it cancel timerfd,need to reset it
				{
					if(timer.is_repeat())
					// FIXME
					// 同heap.empty的情况，timer.repeat后settimer就出函数
					{
						timer.repeat();
						if (comp(old_top_ptr, s_ptr)) // the new one is earlier
						{
							settimer(timer);
						}
						else // reset old_top timer
						{
							settimer(*old_top_ptr);
						}
						heap_.push_heap(s_ptr); // it always need to push in
					}
					else // just reset old_top timer
					{
						settimer(*old_top_ptr);
					}
				}
			}
			else
			{
				heap_.push_heap(s_ptr);
			}
		}
	}


private:
	void tick()
	{
		Timer& timeout_timer=*(heap_.get_top());
		timeout_timer.timeout(timerfd_);
		if (timeout_timer.is_repeat())
		{
			// TODO
			timeout_timer.repeat();
			if (heap_.size()==1)
			{
				settimer(timeout_timer);
			}
			else
			{
				TimerCompare comp;
				if (comp(heap_.second(), heap_.get_top())) // the top one is still earlier
				{
					settimer(timeout_timer);
				}
				else
				{
					std::shared_ptr<Timer> s_ptr=heap_.get_top();
					heap_.pop_heap();
					heap_.push_heap(s_ptr);
					Timer& new_top=*(heap_.get_top());
					settimer(new_top);
				}
			}
		}
		else // not repeat
		{
			heap_.pop_heap();
			if (!heap_.empty())
			{
				Timer& next_timer=*(heap_.get_top());
				settimer(next_timer);
			}
		}
	}

	bool settimer(Timer& timer)
	{
		itimerspec interval;
		memset(&interval, 0, sizeof interval);
		timespec timer_time=timer.get_time();
		timespec now;
		::clock_gettime(CLOCK_MONOTONIC, &now);
		interval.it_value.tv_nsec=timer_time.tv_nsec-now.tv_nsec;
		if (interval.it_value.tv_nsec<0)
		{
			if (timer_time.tv_sec<=now.tv_sec) // already timeout
			{
				timer.timeout(timerfd_);

				return false;
			}
			interval.it_value.tv_nsec+=1000000000;
			interval.it_value.tv_sec=(timer_time.tv_sec-now.tv_sec)-1;
		}
		else
		{
			if (timer_time.tv_sec<now.tv_sec) // already timeout
			{
				timer.timeout(timerfd_);

				return false;
			}
			interval.it_value.tv_sec=timer_time.tv_sec-now.tv_sec;
		}

		::timerfd_settime(timerfd_, 0, &interval, NULL);
		return true;
	}

	int timerfd_;
	help::heap<std::shared_ptr<Timer>,TimerCompare> heap_;
};

} // namespace netthreads

#endif // !NETTHREADS_TIMERHEAP_HPP
