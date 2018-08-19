#ifndef NETTHREADS_TIMEDATA_H
#define NETTHREADS_TIMEDATA_H

#include<time.h>
#include<memory>


namespace netthreads
{

using TimeData=timespec;

inline TimeData make_timedata(time_t seconds, long nanoseconds)
{
	TimeData ret;
	ret.tv_sec=seconds;
	ret.tv_nsec=nanoseconds;
	
	return ret;
}

TimeData culculate_time(TimeData interval)
{
	TimeData ret;
	if (clock_gettime(CLOCK_MONOTONIC, &ret)==-1)
	{
		// TODO: log here

	}
	ret.tv_sec+=interval.tv_sec;
	ret.tv_nsec+=interval.tv_nsec;

	return ret;
}

TimeData culculate_time(time_t seconds, long nanoseconds)
{
	return culculate_time(make_timedata(seconds,nanoseconds));
}

enum TimerType
{
	At,
	After,
	Every
};

class Timer
{
public:
	Timer(TimeData timedata,Callback cb,TimerType type)
		:cb_(cb)
	{
		if (type==At)
		{
			time_=timedata;
			repeat_=false;
		}
		else if(type==After)
		{
			time_=culculate_time(timedata);
			repeat_=false;
		}
		else // Every
		{
			assert((timedata.tv_sec>0)||(timedata.tv_nsec>100000)); // 最小间隔为100微秒
			time_=culculate_time(timedata);
			interval_=timedata;
			repeat_=true;
		}
	}

	Timer(time_t seconds, long nanoseconds, Callback cb, TimerType type)
		:cb_(cb)
	{
		if (type==After)
		{
			time_=culculate_time(seconds, nanoseconds);
			repeat_=false;
		}
		else // Every
		{
			assert((seconds>0)||(nanoseconds>100000));
			time_=culculate_time(seconds, nanoseconds);
			interval_=make_timedata(seconds, nanoseconds);
			repeat_=true;
		}
	}

	TimeData get_time() const
	{
		return time_;
	}

	void set_time(TimeData time)
	{
		time_=time;
	}

	TimeData get_interval() const
	{
		return interval_;
	}

	void set_interval(TimeData interval)
	{
		interval_=interval;
	}

	void repeat() // 刷新time_
	{
		assert(repeat_);

		time_.tv_sec+=interval_.tv_sec;
		time_.tv_nsec+=interval_.tv_nsec;
	}

	void cancel()
	{
		cb_=nullptr;
		repeat_=false;
	}

	void timeout(int timerfd) const
	{
		cb_(timerfd);
	}

	bool is_repeat() const
	{
		return repeat_;
	}

private:
	TimeData time_; // the timespec here stored the time culculate from Linux start
	Callback cb_;
	TimeData interval_;
	bool repeat_;
};

using TimerHandle=std::weak_ptr<Timer>;

} // namespace netthreads

#endif // !NETTHREADS_TIMEDATA_H
