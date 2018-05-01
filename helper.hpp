#ifndef HELPER_HPP
#define HELPER_HPP

#include<vector>
#include<algorithm>
#include<functional>



namespace help
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

template<typename DataType,typename Compare=std::less<>>
class heap
{
public:
	heap()
		:comp_(Compare())
	{   }

	heap(std::vector<DataType>& data)
		:vector_(data), comp_(Compare())
	{

	}

	void make_heap()
	{
		std::make_heap(vector_.begin(), vector_.end(), comp_);
	}

	void push_heap(DataType data)
	{
		vector_.push_back(data);
		std::push_heap(vector_.begin(), vector_.end(), comp_);
	}

	void push_heap(DataType&& data)
	{
		vector_.push_back(std::move(data));
		std::push_heap(vector_.begin(), vector_.end(), comp_);
	}

	void pop_heap()
	{
		if (vector_.empty())
			return;

		std::pop_heap(vector_.begin(), vector_.end(), comp_);
		auto lastelement_it=--(vector_.end());
		vector_.erase(lastelement_it);
	}

	const DataType& get_top() const
	{
		return vector_.front();
	}

	std::vector<DataType> get_data() const
	{
		return vector_;
	}

	bool empty() const
	{
		return vector_.empty();
	}

private:
	std::vector<DataType> vector_;
	Compare comp_;
};


} // namespace netthreads


#endif // !HELPER_HPP
