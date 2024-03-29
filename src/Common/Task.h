#ifndef LOTOSPP_COMMON_TASK_H
#define LOTOSPP_COMMON_TASK_H

#include <boost/function.hpp>
#include <boost/thread/thread_time.hpp>


namespace LotosPP::Common {


class Task
{
public:
	// DO NOT allocate this class on the stack
	Task(uint32_t ms, const boost::function<void (void)>& f)
		: m_expiration{boost::get_system_time()+boost::posix_time::milliseconds(ms)}, m_f{f}
	{};
	Task(const boost::function<void (void)>& f)
		: m_expiration{boost::date_time::not_a_date_time}, m_f{f}
	{};

	~Task()
	{};

	void operator()() const
	{
		m_f();
	};

	void setDontExpire()
	{
		m_expiration=boost::date_time::not_a_date_time;
	};

	bool hasExpired() const
	{
		if (m_expiration==boost::date_time::not_a_date_time) {
			return false;
			}
		return m_expiration<boost::get_system_time();
	};
protected:
	// Expiration has another meaning for scheduler tasks,
	// then it is the time the task should be added to the dispatcher
	boost::system_time m_expiration;
	boost::function<void (void)> m_f;
};

inline Task* createTask(boost::function<void (void)> f)
{
	return new Task(f);
}

inline Task* createTask(uint32_t expiration, boost::function<void (void)> f)
{
	return new Task(expiration, f);
}

	}

#endif
