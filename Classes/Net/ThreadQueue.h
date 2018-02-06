/*
*Header File:
*@author emps
*
*/

#ifndef _H_BLOCKING_QUEUE_
#define _H_BLOCKING_QUEUE_
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadQueue
{
public:

	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		auto val = queue_.front();
		queue_.pop();
		mlock.unlock();
		return val;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		item = queue_.front();
		queue_.pop();
		mlock.unlock();
	}

	int size()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		int c = queue_.size();
		mlock.unlock();
		return c;
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
	}

	ThreadQueue() = default;
	ThreadQueue(const ThreadQueue&) = delete;            // disable copying
	ThreadQueue& operator=(const ThreadQueue&) = delete; // disable assignment

private:
	std::queue<T> queue_;
	std::mutex mutex_;
};

#endif