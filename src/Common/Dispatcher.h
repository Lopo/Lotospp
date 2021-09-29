#ifndef LOTOSPP_COMMON_DISPATCHER_H
#define LOTOSPP_COMMON_DISPATCHER_H

#include <boost/thread.hpp>
#include <list>


namespace LotosPP::Common {
	class Task;

const int DISPATCHER_TASK_EXPIRATION=2000;

class Dispatcher
{
public:
	Dispatcher();
	~Dispatcher()
	{};

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

	std::list<Task*> m_taskList{};
	DispatcherState m_threadState{STATE_TERMINATED};
};

extern Dispatcher g_dispatcher;

	}

#endif
