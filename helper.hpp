#ifndef HELPER_HPP
#define HELPER_HPP

namespace netthreads
{

class noncopyable
{
protected:
	noncopyable()
	{   }

	~noncopyable()
	{   }

	noncopyable(noncopyable&)=delete;
	noncopyable& operator=(noncopyable&)=delete;

};




}

#endif // !HELPER_HPP
