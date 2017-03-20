#ifndef _BASE_UTIL_QUEUE_H_
#define _BASE_UTIL_QUEUE_H_
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <queue>
#include "platform.h"

template <typename T>
class CThreadQueue
{
public:
	CThreadQueue(){};
	~CThreadQueue(){};

	bool enqueue(const T &d)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_queue.push(d);
		m_cond.notify_one();
		return true;
	}

	bool deque(T &d, int64 wait_ms = -1)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		if (m_queue.empty())
		{
			if (wait_ms == 0)
			{
				return false;
			}
			else if (wait_ms >0)
			{
				//m_cond.timed_wait(lock, boost::posix_time::microsec(wait_ms));
				if (m_queue.empty())
				{
					return false;
				}
			}
			else
			{
				m_cond.wait(lock);
			}
		}
		d = m_queue.front();
		m_queue.pop();
		return true;
	}

	void wakeupall()
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_cond.notify_all();
	}

	uint32 size()
	{
		boost::mutex::scoped_lock lock(m_mutex);
		return m_queue.size();
	}

private:

	std::queue<T> m_queue;
	boost::condition_variable m_cond;
	boost::mutex m_mutex;
};


#endif