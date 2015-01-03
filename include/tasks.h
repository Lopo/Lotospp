#ifndef LOTOS2_TASKS_H
#define LOTOS2_TASKS_H

#include "config.h"

#include <boost/function.hpp>
#include <boost/thread.hpp>


namespace lotos2 {

const int DISPATCHER_TASK_EXPIRATION=2000;


class Task
{
public:
	// DO NOT allocate this class on the stack
	Task(uint32_t ms, const boost::function<void (void)>& f)
		: m_f(f)
	{
		m_expiration=boost::get_system_time()+boost::posix_time::milliseconds(ms);
	};
	Task(const boost::function<void (void)>& f)
		: m_expiration(boost::date_time::not_a_date_time), m_f(f) {};

	~Task() {};

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
};

inline Task* createTask(uint32_t expiration, boost::function<void (void)> f)
{
	return new Task(expiration, f);
};

enum DispatcherState {
	STATE_RUNNING,
	STATE_CLOSING,
	STATE_TERMINATED
	};

class Dispatcher
{
public:
	Dispatcher();
	~Dispatcher() {};

	void addTask(Task* task, bool push_front=false);

	void start();
	void stop();
	void shutdown();
	void shutdownAndWait();

	enum DispatcherState {
		STATE_RUNNING,
		STATE_CLOSING,
		STATE_TERMINATED
		};

protected:
	static void dispatcherThread(void* p);

	void flush();

	boost::thread m_thread;
	boost::mutex m_taskLock;
	boost::condition_variable m_taskSignal;

	std::list<Task*> m_taskList;
	DispatcherState m_threadState;
};

extern Dispatcher g_dispatcher;

} // namespace lotos2

#endif // LOTOS2_TASKS_H
