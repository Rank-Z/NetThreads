#ifndef HELPER_HPP
#define HELPER_HPP

#include<string>
#include<vector>
#include<algorithm>
#include<functional>
#include<exception>


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

class NT_exception : public::std::exception
{
public:
	NT_exception(std::string reason)
		:reason_(reason)
	{   }

	const char* what() const noexcept
	{
		return reason_.c_str();
	}
private:
	std::string reason_;
};


template<typename DataType,typename Compare>
class heap
{
public:
	heap()
		:comp_(Compare())
	{   }

	heap(std::vector<DataType>& data)
		:vector_(data), comp_(Compare())
	{
		if (!std::is_heap(vector_.begin(), vector_.end(), comp_))
			make_heap();
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
		return vector_.data();
	}

	bool empty() const
	{
		return vector_.empty();
	}

	DataType& second()
	{
		assert(vector_.size()>1);
		if (vector_.size()==2)
		{
			return *(++vector_.begin());
		}
		else
		{
			auto it=vector_.begin();
			++it;
			DataType& data2=*it;
			++it;
			DataType& data3=*it;

			if (comp_(data2, data3))
			{
				return data3;
			}
			else
			{
				return data2;
			}
		}
	}

	int size() const
	{
		return vector_.size();
	}

private:
	std::vector<DataType> vector_;
	Compare comp_;
};


} // namespace netthreads


#endif // !HELPER_HPP
